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
import signal

CACHE_DIR = '.run_verilator_tests_cache'
# LOG_FILE_NAME = 'tests.log'
# LOG_FORMAT = '%(asctime)s - %(message)s'
LOG_FORMAT = '{} - {}'
LOG_DATE_FORMAT = '%d-%m-%Y %H:%M:%S'
FAILED_CACHE = os.path.join(CACHE_DIR, 'failed')
ALL_CACHE = os.path.join(CACHE_DIR, 'all')
REMAINING_CACHE = os.path.join(CACHE_DIR, 'remaining')

ERROR_PATTERN = 'error\s+(\d+)'
# Unfortunately hardcoded for now
TB_BIN_DIR = '../bin'

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
            self.cache_all = lambda testnames: None
            self.cache_failed_test = lambda testname: None
            self.cache_remaining = lambda test_names: None
            self.close = lambda: None
        else:
            os.makedirs(CACHE_DIR, exist_ok=True)
            self.all_cache = open(ALL_CACHE, 'w')
            self.failed_cache = open(FAILED_CACHE, 'w')
            self.remaining_cache = open(REMAINING_CACHE, 'w')
            self.cache_failed_test = lambda testname : print(testname, file=self.failed_cache)
            self._files = [self.all_cache, self.failed_cache, self.remaining_cache]
            
            def _close():
                for f in self._files:
                    f.close()
            
            self.close = _close

            def _cache_all(test_names: list):
                for test_name in test_names:
                    print(test_name.strip(), file=self.all_cache)
            self.cache_all = _cache_all

            def _cache_remaining(test_names: list):
                for test_name in test_names:
                    print(test_name.strip(), file=self.remaining_cache)
            self.cache_remaining = _cache_remaining

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

        path = os.path.join(self.log_dir, name + '.logs')
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


def signal_handler_factory(trace_names: list, cache: RunCache):
    def _handler(sig, frame):
        cache.cache_remaining(trace_names)
        cache.close()
        sys.exit(0)

    return _handler

def get_test_list(log, args):
    test_list_file = ''
    if args.command == 'list':
        test_list_file = args.test_list
    elif args.command == 'all':
        test_list_file = 'test_lists/all_tests.txt'
    elif args.command == 'last_run' or args.command == 'last':
        if args.failed:
            test_list_file = FAILED_CACHE
        elif args.remaining:
            test_list_file = REMAINING_CACHE
        else:
            test_list_file = ALL_CACHE
        
        if not os.path.exists(test_list_file):
            log.info('No run cached -- cannot use the "last/last_run" option')
            sys.exit(1)
    f = open(test_list_file, 'r')
    test_list = f.readlines()
    f.close()

    return test_list_file, test_list
                
def test_it(test_list: list):
    while(test_list):
        yield test_list[0]
        test_list.pop(0)


common = ArgumentParser(add_help=False)

parser = ArgumentParser(description='Program for running lists of nvdla trace tests with the Verilator NVDLA testbench.',
                        formatter_class=RawTextHelpFormatter)

common.add_argument('-v', '--verbose',
                    action='store_true',
                    help='Make log verbose by writing test output to stdout.')
common.add_argument('-l', '--log',
                    required=False,
                    type=str,
                    help='Specify directory where logs will be saved, if you want any.')

common.add_argument('-r', '--nvdla_root',
                    default='../../../nvdla_hw',
                    help='The root folder of the NVDLA HW project; DEFAULT=../../../nvdla_hw')

common.add_argument('--no-cache',
                    action='store_true',
                    default=False,
                    dest='no_cache',
                    help='Do not save this run (will not be used when using the "last_run" command)')

common.add_argument('testbench',
                    help='Name of the testbench you want to test')
common.add_argument('--vcd',
		    required=False,
		    type=str,
		    help='Directory where VCD traces will be saved')

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
last_run_parser.add_argument('--remaining', action='store_true', help='Run the tests which should have been run, but the program was interrupted')

parser.epilog = "--- Arguments common to all commands ---" + common.format_help().replace(common.format_usage(), '')

args = parser.parse_args()

testbench = args.testbench

error_pattern = re.compile(ERROR_PATTERN, re.IGNORECASE)
log = Log(args)

nvdla_root    = os.path.abspath(args.nvdla_root)
verilator_dir = os.path.join(nvdla_root, 'verif', 'verilator')

test_list, tests = get_test_list(log, args)
cache = RunCache(args)

signal_handler = signal_handler_factory(tests, cache)
signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

log.debug('Running tests from test list: {} ({})'.format(test_list, os.path.abspath(test_list)))

total_test_count = len(tests)
failed_tests     = []
make_error_dict  = defaultdict(list)
return_code_dict = defaultdict(list)


vcd_trace_dir = None
if args.vcd:
    vcd_trace_dir = os.path.join(args.vcd, testbench)
    os.makedirs(vcd_trace_dir, exist_ok=True)

cache.cache_all(tests)

for i, filename in enumerate(test_it(tests), start=1):
    filename = filename.strip()
    test = os.path.join(nvdla_root, 'outdir', 'nv_small', 'verilator', 'test', filename, 'trace.bin')
    
    log.add_logfile(filename, LogLevel.DEBUG)

    log.test_pass_start('Running test ({}/{}): {} '.format(i, total_test_count, filename))

    test_failed_rc   = False
    test_failed_make = False
    make_error = None
    return_code = 0

    testbench_path = os.path.join(TB_BIN_DIR, testbench)
    # testbenches_root = '../'
    # simulation = os.path.join(testbenches_root, testbench, 'build', testbench)
    sim_args = [testbench_path, test]
    if vcd_trace_dir:
        trace_loc = os.path.join(vcd_trace_dir, filename + '.vcd')
        sim_args.append(trace_loc)

    with sp.Popen(sim_args,
                  stdout=sp.PIPE,
                  stderr=sp.PIPE,
                  #cwd=verilator_dir,
                  text=True) as p:

        for out, err in read_pipes(p):

            if out:
                out = out.strip()
                log.debug(out)
                
            if err:
                err = err.strip()
                log.debug(err)
                
                error_search_result = error_pattern.search(err)

                if error_search_result:
                    test_failed_make = True
                    make_error = error_search_result.group(1)


        return_code = p.poll()
        if return_code is not None and return_code != 0:
            test_failed_rc = True

    test_failed = test_failed_make or test_failed_rc

    log.test_pass_end(test_failed)

    if test_failed:
        failed_tests.append(filename)
        cache.cache_failed_test(filename)
    
    if test_failed_make:
        make_error_dict[make_error].append(filename)
    elif test_failed_rc:
        return_code_dict[return_code].append(filename)
    
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
