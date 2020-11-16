#include "kmem_ctx/kmem_ctx.hpp"

int __cdecl main(int argc, char** argv)
{
	vdm::read_phys_t _read_phys = [&](void* addr, void* buffer, std::size_t size) -> bool
	{
		return vdm::read_phys(addr, buffer, size);
	};

	vdm::write_phys_t _write_phys = [&](void* addr, void* buffer, std::size_t size) -> bool
	{
		return vdm::write_phys(addr, buffer, size);
	};

	auto kmem_handle = nasa::kmem_ctx::get_handle();

	// translation just subtracts pml4 index bit field by 255...
	const auto ntoskrnl_base = util::get_kmodule_base("ntoskrnl.exe");
	const auto ntoskrnl_translated = nasa::kmem_ctx::translate(ntoskrnl_base);

	std::printf("[+] ntoskrnl base -> 0x%p\n", ntoskrnl_base);
	std::printf("[+] ntoskrnl translated -> 0x%p\n", ntoskrnl_translated);

	if (kmem_handle == INVALID_HANDLE_VALUE)
	{
		const auto [drv_handle, drv_key] = vdm::load_drv();
		if (drv_handle == INVALID_HANDLE_VALUE)
		{
			std::printf("[!] invalid handle...\n");
			std::getchar();
			return -1;
		}

		vdm::vdm_ctx vdm(_read_phys, _write_phys);
		nasa::kmem_ctx kmem(&vdm);

		kmem_handle = kmem.get_handle();
		unsigned short mz = 0u;
		std::size_t bytes_handled;

		// ReadProcessMemory kernel memory example...
		auto result =
			ReadProcessMemory(
				kmem_handle,
				reinterpret_cast<void*>(ntoskrnl_translated),
				&mz, sizeof mz,
				&bytes_handled
			);

		std::printf("[+] ReadProcessMemory Result -> %d, mz -> 0x%x\n", result, mz);

		if (!vdm::unload_drv(drv_handle, drv_key))
		{
			std::printf("[!] unable to unload driver...\n");
			std::getchar();
			return -1;
		}
	}
	else
	{
		unsigned short mz = 0u;
		std::size_t bytes_handled;

		// ReadProcessMemory kernel memory example...
		auto result =
			ReadProcessMemory(
				kmem_handle,
				reinterpret_cast<void*>(ntoskrnl_translated),
				&mz, sizeof mz,
				&bytes_handled
			);

		std::printf("[+] ReadProcessMemory Result -> %d, mz -> 0x%x\n", result, mz);
	}
	std::printf("[+] press enter to exit...\n");
	std::getchar();
}