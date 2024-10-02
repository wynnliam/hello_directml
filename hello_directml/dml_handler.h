// Liam Wynn, 10/1/2024, Hello DirectML

#pragma once

#include "stdafx.h"

// I'm not really going to get into what these do. If you're
// interested, look at my Hello DirectX 12 repo.
struct dml_handler {
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> command_queue;
	ComPtr<ID3D12CommandAllocator> command_allocator;
	ComPtr<ID3D12GraphicsCommandList> command_list;
};

bool initialize_dml_handler(dml_handler* dml);
void enable_dx12_debug_layer();
ComPtr<IDXGIFactory4> create_dx12_factory();

ComPtr<IDXGIAdapter4> get_valid_adapter(
	const bool use_warp,
	ComPtr<IDXGIFactory4> factory
);
ComPtr<IDXGIAdapter4> get_warp_adapter(ComPtr<IDXGIFactory4> factory);
ComPtr<IDXGIAdapter4> get_nonwarp_adapter(ComPtr<IDXGIFactory4> factory);

ComPtr<ID3D12Device> create_dx12_device(ComPtr<IDXGIAdapter4> adapter);
ComPtr<ID3D12CommandQueue> create_command_queue(dml_handler* dml);
ComPtr<ID3D12CommandAllocator> create_command_allocator(
	dml_handler* dml,
	D3D12_COMMAND_LIST_TYPE command_list_type
);
ComPtr<ID3D12GraphicsCommandList> create_command_list(
	dml_handler* dml,
	D3D12_COMMAND_LIST_TYPE command_list_type
);
