// Liam Wynn, 10/1/2024, Hello DirectML

#include "dml_handler.h"

#include "utils.h"

bool initialize_dml_handler(dml_handler* dml) {
	ComPtr<IDXGIFactory4> factory;
	ComPtr<IDXGIAdapter4> adapter;

	enable_dx12_debug_layer();

	factory = create_dx12_factory();
	adapter = get_valid_adapter(false, factory);

	dml->device = create_dx12_device(adapter);
	dml->command_queue = create_command_queue(dml);
	dml->command_allocator = create_command_allocator(
		dml,
		D3D12_COMMAND_LIST_TYPE_DIRECT
	);
	dml->command_list = create_command_list(
		dml,
		D3D12_COMMAND_LIST_TYPE_DIRECT
	);

	return true;
}

void enable_dx12_debug_layer() {
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug_interface;
	HRESULT result;

	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface));
	throw_if_failed(result);

	debug_interface->EnableDebugLayer();

#endif
}

ComPtr<IDXGIFactory4> create_dx12_factory() {
	ComPtr<IDXGIFactory4> factory;
	UINT factory_flags;
	HRESULT result;

	factory_flags = 0;
#if defined(_DEBUG)
	factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	result = CreateDXGIFactory2(
		factory_flags,
		IID_PPV_ARGS(&factory)
	);

	throw_if_failed(result);

	return factory;
}

ComPtr<IDXGIAdapter4> get_valid_adapter(
	const bool use_warp,
	ComPtr<IDXGIFactory4> factory
) {
	ComPtr<IDXGIAdapter4> adapter;

	if (use_warp) {
		return get_warp_adapter(factory);
	} else {
		return get_nonwarp_adapter(factory);
	}
}

ComPtr<IDXGIAdapter4> get_warp_adapter(ComPtr<IDXGIFactory4> factory) {
	HRESULT result;
	ComPtr<IDXGIAdapter1> adapter_1;
	ComPtr<IDXGIAdapter4> adapter_4;

	result = factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter_1));
	throw_if_failed(result);

	result = adapter_1.As(&adapter_4);
	throw_if_failed(result);

	return adapter_4;
}

ComPtr<IDXGIAdapter4> get_nonwarp_adapter(ComPtr<IDXGIFactory4> factory) {
	ComPtr<IDXGIAdapter4> adapter_4;
	ComPtr<IDXGIAdapter1> adapter_1;
	HRESULT result;
	UINT adapter_index;

	adapter_index = 0;

	do {
		result = factory->EnumAdapters1(adapter_index, &adapter_1);
		throw_if_failed(result);
		adapter_index++;

		result = D3D12CreateDevice(
			adapter_1.Get(),
			D3D_FEATURE_LEVEL_11_0,
			__uuidof(ID3D12Device),
			NULL
		);

		if (result == DXGI_ERROR_UNSUPPORTED) {
			continue;
		}

		throw_if_failed(result);

	// D3D12CreateDevice will return S_FALSE if it can create a
	// device, but the device pointer is NULL. We set it to NULL,
	// so we want to check that S_FALSE is a success.
	} while (result != S_OK && result != S_FALSE);

	result = adapter_1.As(&adapter_4);
	throw_if_failed(result);

	return adapter_4;
}

ComPtr<ID3D12Device> create_dx12_device(ComPtr<IDXGIAdapter4> adapter) {
	ComPtr<ID3D12Device> device;
	HRESULT result;

#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> debug_info;
	D3D12_INFO_QUEUE_FILTER filter;

	D3D12_MESSAGE_SEVERITY ignore_severities[] = {
		D3D12_MESSAGE_SEVERITY_INFO
	};

	D3D12_MESSAGE_ID ignore_ids[] = {
		D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
	};

	filter = {};
	filter.DenyList.NumSeverities = _countof(ignore_severities);
	filter.DenyList.pSeverityList = ignore_severities;
	filter.DenyList.NumIDs = _countof(ignore_ids);
	filter.DenyList.pIDList = ignore_ids;
#endif

	result = D3D12CreateDevice(
		adapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device)
	);

	throw_if_failed(result);

#if defined(_DEBUG)
	result = device.As(&debug_info);
	if (SUCCEEDED(result)) {
		debug_info->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		debug_info->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		debug_info->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}

	result = debug_info->PushStorageFilter(&filter);
	throw_if_failed(result);
#endif

	return device;
}

ComPtr<ID3D12CommandQueue> create_command_queue(dml_handler* dml) {
	ComPtr<ID3D12CommandQueue> command_queue;
	ComPtr<ID3D12Device> device;
	D3D12_COMMAND_QUEUE_DESC desc;
	HRESULT result;

	device = dml->device;

	desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	result = device->CreateCommandQueue(
		&desc,
		IID_PPV_ARGS(&command_queue)
	);

	throw_if_failed(result);

	return command_queue;
}

ComPtr<ID3D12CommandAllocator> create_command_allocator(
	dml_handler* dml,
	D3D12_COMMAND_LIST_TYPE command_list_type
) {
	ComPtr<ID3D12CommandAllocator> allocator;
	ComPtr<ID3D12Device> device;
	HRESULT result;

	device = dml->device;
	result = device->CreateCommandAllocator(
		command_list_type,
		IID_PPV_ARGS(&allocator)
	);

	throw_if_failed(result);

	return allocator;
}

ComPtr<ID3D12GraphicsCommandList> create_command_list(
	dml_handler* dml,
	D3D12_COMMAND_LIST_TYPE command_list_type
) {
	ComPtr<ID3D12GraphicsCommandList> command_list;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> allocator;
	HRESULT result;

	device = dml->device;
	allocator = dml->command_allocator;

	result = device->CreateCommandList(
		0,
		command_list_type,
		allocator.Get(),
		NULL,
		IID_PPV_ARGS(&command_list)
	);

	throw_if_failed(result);

	return command_list;
}