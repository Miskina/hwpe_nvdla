#!/usr/bin/python

import subprocess as sp
import os
import sys
from argparse import ArgumentParser, RawTextHelpFormatter
from datetime import datetime
import re
from queue import Queue, Empty
from concurrent.futures import ThreadPoolExecutor
from collections import defaultdict
from enum import IntEnum

CACHE_DIR = '.run_questa_tests_cache'
# LOG_FILE_NAME = 'tests.log'
# LOG_FORMAT = '%(asctime)s - %(message)s'
LOG_FORMAT = '{} - {}'
LOG_DATE_FORMAT = '%d-%m-%Y %H:%M:%S'
FAILED_CACHE = os.path.join(CACHE_DIR, 'failed')
ALL_CACHE = os.path.join(CACHE_DIR, 'all')

ERROR_PATTERN = re.compile('error\s+(\d+)', re.IGNORECASE)
REGISTER_SETUP_FAIL_PATTERN = re.compile('\s*Registers\s*setup\s*failed.*', re.IGNORECASE)
TEST_FAIL_PATTERN = re.compiile('Trace\s*failed', re.IGNORECASE)


class TerminalColors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class RunCache:

    def __init__(self, args):
        if args.no_cache:
            self.cache_test = lambda testname: None
            self.cache_failed_test = lambda testname: None
            self.close = lambda: None
        else:
            os.makedirs(CACHE_DIR, exist_ok=True)
            self.all_cache = open(ALL_CACHE, 'w')
            self.failed_cache = open(FAILED_CACHE, 'w')
            self.cache_test = lambda testname : print(testname, file=self.all_cache)
            self.cache_failed_test = lambda testname : print(testname, file=self.failed_cache)
            self._files = [self.all_cache, self.failed_cache]
            
            def _close():
                self.all_cache.close()
                self.failed_cache.close()
            
            self.close = lambda: _close()

class LogLevel(IntEnum):
    INFO  = 1
    DEBUG = 0

class Log:

    def __init__(self, args):        
        self.verbose = args.verbose
        self.log_dir = args.log
        
        self.logfiles = {}

        self.root_level = LogLevel.INFO
        if self.verbose:
            self.root_level = LogLevel.DEBUG
        
        
        if self.log_dir:
            os.makedirs(self.log_dir, exist_ok=True)
            self.add_logfile('tests', level=LogLevel.DEBUG)


    def _get_log_msg(self, msg):
        time = datetime.now().strftime(LOG_DATE_FORMAT)
        msg = LOG_FORMAT.format(time, msg)
        return msg

    def _log_msg(self, msg, msg_lvl, log_lvl, file=sys.stdout):
        if log_lvl <= msg_lvl:
            print(msg, file=file)

    def test_pass_start(self, msg, level=LogLevel.INFO):
        msg = self._get_log_msg(msg)

        if self.root_level <= level:
            print(msg, end='')
            sys.stdout.flush()
        
        for logfile, logfile_level, _ in self.logfiles.values():
            if logfile_level <= level:
                print(msg, end='', file=logfile)

    def test_pass_end(self, test_failed, level=LogLevel.INFO):
        msg = 'failed!' if test_failed else 'passed!'
        stdout_msg = TerminalColors.FAIL if test_failed else TerminalColors.OKGREEN
        stdout_msg += msg + TerminalColors.ENDC

        self._log_msg(stdout_msg, level, self.root_level)
        
        for logfile, logfile_level, _ in self.logfiles.values():
            self._log_msg(msg, level, logfile_level, logfile)


    def info(self, msg):
        msg  = self._get_log_msg(msg)
        self._log_msg(msg, LogLevel.INFO, self.root_level)
        for logfile, level, path in self.logfiles.values():
            self._log_msg(msg, LogLevel.INFO, level, logfile)
                

    def debug(self, msg):
        msg  = self._get_log_msg(msg)
        self._log_msg(msg, LogLevel.DEBUG, self.root_level)
        for logfile, level, path in self.logfiles.values():
            self._log_msg(msg, LogLevel.DEBUG, level, logfile)
            

    def _add_logfile_impl(self, name, path, level):
        logfile = open(path, 'w')
        self.logfiles[name] = (logfile, level, path)

    def add_logfile(self, name, level):
        if not self.log_dir:
            return

        path = os.path.join(self.log_dir, name + '.log')
        self._add_logfile_impl(name, path, level)

    
    def remove_logfile(self, name):
        if not self.log_dir:
            return

        logfile, level, path = self.logfiles.pop(name)
        logfile.close()

    def close(self):
        for logfile, level, path in self.logfiles.values():
            logfile.close()



def enq_out(file, queue):
    for line in iter(file.readline, ''):
        queue.put(line)
    file.close()


def log_err_list(log, filename, msg, entries):
    if not entries:
        return
    log.debug(msg)
    with open(filename, 'w') as f:
        for entry in entries:
            print(entry, file=f)


def read_pipes(p):

    with ThreadPoolExecutor(2) as pool:
        out_q, err_q = Queue(), Queue()

        pool.submit(enq_out, p.stdout, out_q)
        pool.submit(enq_out, p.stderr, err_q)

        while True:
            
            return_code = p.poll()

            if return_code is not None and out_q.empty() and err_q.empty():
                break

            out, err = '', ''

            try:
                out = out_q.get_nowait()
            except Empty:
                pass

            try:
                err = err_q.get_nowait()
            except Empty:
                pass
                
            
            # if return_code is not None and out == '' and err == '':
            #     break

            yield (out, err)


def get_test_list(log, args):
    test_list_file = ''
    if args.command == 'list':
        test_list_file = args.test_list
    elif args.command == 'all':
        test_list_file = '../../verif/tests/test_lists/all_tests.txt'
    elif args.command == 'last_run' or args.command == 'last':
        if args.failed:
            test_list_file = os.path.join(CACHE_DIR, 'failed')
        else:
            test_list_file = os.path.join(CACHE_DIR, 'all')
        
        if not os.path.exists(test_list_file):
            log.info('No run cached -- cannot use the "last/last_run" option')
            sys.exit(1)
    f = open(test_list_file, 'r')
    test_list = f.readlines()
    f.close()

    return test_list_file, test_list
                


common = ArgumentParser(add_help=False)

parser = ArgumentParser(description='Program for running lists of nvdla trace tests with Questasim.',
                        formatter_class=RawTextHelpFormatter)

common.add_argument('-v', '--verbose',
                    action='store_true',
                    help='Make log verbose by writing test output to stdout.')
common.add_argument('-l', '--log',
                    required=False,
                    type=str,
                    help='Specify directory where logs will be saved, if you want any.')

common.add_argument('-t', '--test_folder',
                    default='.',
                    help='The folder where the C test and Makefile are; DEFAULT=.')

common.add_argument('--no-cache',
                    action='store_true',
                    help='Do not save this run (will not be used when using the "last_run" command)')

subparsers = parser.add_subparsers(title='Run options', required=True, description='Multiple ways of specifiying which tests you want to run.', dest='command')


list_parser = subparsers.add_parser('list',
                                    description='Specify a list of test names you want to run (a text file with the test names as lines).',
                                    parents=[common])
list_parser.add_argument('test_list', default='../../verif/tests/test_lists/all_tests.txt')
 

all_parser = subparsers.add_parser('all',
                                    description='Shortcut for running the test list with all of the tests in it.',
                                    parents=[common])

last_run_parser = subparsers.add_parser('last_run',
                                        aliases=['last'],
                                        description='Run tests ran in the last run of the script',
                                        parents=[common])
last_run_parser.add_argument('--failed', action='store_true', help='Run only the tests that failed last time')

parser.epilog = "--- Arguments common to all commands ---" + common.format_help().replace(common.format_usage(), '')

args = parser.parse_args()

log = Log(args)

nvdla_root    = args.nvdla_root
test_folder = args.test_folder 

test_list, tests = get_test_list(log, args)

log.debug('Running tests from test list: {} ({})'.format(test_list, os.path.abspath(test_list)))

total_test_count = len(tests)
failed_tests     = []
make_error_dict  = defaultdict(list)
return_code_dict = defaultdict(list)
setup_register_list = []
trace_fail_list = []
cache = RunCache(args)

for i, filename in enumerate(tests, start=1):
    filename = filename.strip()
    
    log.add_logfile(filename, LogLevel.DEBUG)

    log.info('Building test: {}', filename)
    sp.run(['/usr/bin/make', 'clean', 'all'], cwd=test_folder)

    log.test_pass_start('Running test ({}/{}): {} '.format(i, total_test_count, filename))

    test_failed_rc   = False
    test_failed_make = False
    test_failed_setup_registers = False
    test_failed_trace = False
    make_error = None
    return_code = 0
    

    with sp.Popen(['/usr/bin/make', 'run', 'trace={}'.format(filename)],
                  stdout=sp.PIPE,
                  stderr=sp.PIPE,
                  cwd=test_folder,
                  text=True) as p:

        for out, err in read_pipes(p):

            if out:
                out = out.strip()
                log.debug(out)
                
            if err:
                err = err.strip()
                log.debug(err)
                
                error_search_result = ERROR_PATTERN.search(err)
                register_fail_result_err = REGISTER_SETUP_FAIL_PATTERN.search(err)
                register_fail_result_out = REGISTER_SETUP_FAIL_PATTERN.search(out)
                test_fail_result_err = TEST_FAIL_PATTERN.search(err)
                test_fail_result_out = TEST_FAIL_PATTERN.search(out)

                if error_search_result:
                    test_failed_make = True
                    make_error = error_search_result.group(1)
                
                test_failed_setup_registers =  register_fail_result_err or register_fail_result_out:
                test_failed_trace = test_fail_result_err or test_fail_result_out


        return_code = p.poll()
        if return_code is not None and return_code != 0:
            test_failed_rc = True

    test_failed = test_failed_make or test_failed_rc or test_failed_setup_registers or test_failed_trace

    log.test_pass_end(test_failed)

    if test_failed:
        failed_tests.append(filename)
        cache.cache_failed_test(filename)
    
    if test_failed_make:
        make_error_dict[make_error].append(filename)
    elif test_failed_rc:
        return_code_dict[return_code].append(filename)
    elif test_failed_setup_registers:
        setup_register_list.append(filename)
    elif test_failed_trace:
        trace_fail_list.append(filename)

    cache.cache_test(filename)

    log.remove_logfile(filename)


log.info('')
log.info('')

if args.log:
    if failed_tests:
        failed_tests_file = os.path.join(args.log, 'failed_tests.txt')
        log_err_list(log, failed_tests_file, 'Saving all failed tests to {}'.format(failed_tests_file), failed_tests)

    for make_err, err_file_names in make_error_dict.items():
        make_err_file = os.path.join(args.log, 'make_error_{}.txt'.format(make_err))
        log_err_list(log, make_err_file, 'Saving tests with Make error: {} to {}'.format(make_err, make_err_file), err_file_names)

    for rc, rc_file_names in return_code_dict.items():
        rc_log_file = os.path.join(args.log, 'return_code_{}.txt'.format(rc))
        log_err_list(log, rc_log_file, 'Saving tests with return code: {} to {}'.format(rc, rc_log_file), rc_file_names)
    
    setup_log_file = os.path.join(args.log, 'setup_registers.txt')
    log_err_list(log, setup_log_file,
        'Saving tests which failed on registers setup to: {}'.format(setup_log_file), setup_log_file)

    trace_err_log_file = os.path.join(args.log, 'trace_err.txt')
    log_err_list(log, trace_err_log_file,
        'Saving tests which failed during trace finish to: {}'.format(trace_err_log_file), trace_fail_list)



cache.close()

log.info('')
log.info('')

__HASHES_20__        = '{:#>20}'.format('')
__HASHES_120__       = '{:#>120}'.format('')
__HASHES_EMPY_LINE__ = '{}{:>100}'.format(__HASHES_20__, __HASHES_20__)

log.info(__HASHES_120__)
log.info(__HASHES_EMPY_LINE__)
log.info('{}{:^80}{}'.format(__HASHES_20__, "All tests finished", __HASHES_20__))
log.info(__HASHES_EMPY_LINE__)


passed_tests_msg = 'Passed {}/{}'.format(total_test_count - len(failed_tests), total_test_count)
log.info(__HASHES_120__)
log.info(__HASHES_EMPY_LINE__)
log.info('{}{:^80}{}'.format(__HASHES_20__, passed_tests_msg, __HASHES_20__))
log.info(__HASHES_EMPY_LINE__)

log.info(__HASHES_120__)
log.info(__HASHES_EMPY_LINE__)
log.info('{}{:^80}{}'.format(__HASHES_20__, "Failed tests:", __HASHES_20__))
for test in failed_tests:
    log.info('{}{:^80}{}'.format(__HASHES_20__, test, __HASHES_20__))


log.info(__HASHES_EMPY_LINE__)
log.info(__HASHES_120__)

log.close()
