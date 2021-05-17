#ifndef HWPE_NVDLA_UTIL_H
#define HWPE_NVDLA_UTIL_H

#include <utility>
#include <cstdio>
#include <type_traits>

#define HWPE_NVDLA_MACRO_START do {
#define HWPE_NVDLA_MACRO_END } while(0)
#define HWPE_NVDLA_COMPONENT_LOG(stream, name, format, ...) fprintf(stream, "[%s] - " format "\n", name, ##__VA_ARGS__)

namespace util
{

	template<typename T>
	inline void delete_if(T* obj) noexcept
	{
		if(obj)
		{
			delete obj;
		}
	}

	template<typename ... Ts>
	struct Release
	{
		constexpr Release(Ts*... objs) noexcept : ptrs(objs...) {} 
		
		~Release() noexcept
		{
			destroy_impl(std::make_index_sequence<sizeof...(Ts)>{});
		}
		
	private:

		template<size_t ... N>
		void destroy_impl(std::index_sequence<N...>) noexcept
		{
			(delete std::get<N>(ptrs), ...);
		}

		std::tuple<Ts*...> ptrs;

	};

	template<typename ... Ts>
	Release(Ts*... args) -> Release<Ts...>;

	template<typename ... VerilatedType>
	struct VerilatedModel
	{
		constexpr VerilatedModel(VerilatedType*... verilatedModels) noexcept : models(verilatedModels...) { }

		~VerilatedModel() noexcept
		{
			destroy_impl(std::make_index_sequence<sizeof...(VerilatedType)>{});
		}

		template<typename VType>
		const VType* get() const noexcept
		{
			return std::get<VType>(models);
		}
		
		template<typename VType>
		VType* get() noexcept
		{
			return std::get<VType>(models);
		}

		void eval() noexcept
		{
			eval_impl(std::make_index_sequence<sizeof...(VerilatedType)>{});
		}

		void final()
		{
			final_impl(std::make_index_sequence<sizeof...(VerilatedType)>{});
		}

		void update_clk() noexcept
		{
			clk = !clk;
		}

	private:

		template<size_t ... N>
		void destroy_impl(std::index_sequence<N...>) noexcept
		{
			(delete std::get<N>(models), ...);
		}

		template<size_t ... N>
		void eval_impl(std::index_sequence<N...>) noexcept
		{
			((std::get<N>(models)->clk = clk), ...);
			(std::get<N>(models)->eval_step(), ...);
			(std::get<N>(models)->eval_end_step(), ...);
		}

		template<size_t ... N>
		void final_impl(std::index_sequence<N...>) noexcept
		{
			(std::get<N>(models)->final(), ...);
		}

		std::tuple<VerilatedType*...> models;
		bool clk = true;
	};

	template<typename ... VerilatedTypes>
	VerilatedModel(VerilatedTypes*... models) -> VerilatedModel<VerilatedTypes...>;
}


#endif // HWPE_NVDLA_UTIL_H