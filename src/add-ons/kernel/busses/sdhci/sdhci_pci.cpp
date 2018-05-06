#include <new>
#include <stdio.h>
#include <string.h>

#include <bus/PCI.h>
#include <device_manager.h>

#include "sdhci_pci.h"

#ifdef TRACE_SDHCI
#	define TRACE(x...) dprintf("\33[33msdhci_pci:\33[0m " x)
#else
#	define TRACE(x...) ;
#endif
#define TRACE_ALWAYS(x...)	dprintf("\33[33msdhci_pci:\33[0m " x)
#define ERROR(x...)			dprintf("\33[33msdhci_pci:\33[0m " x)
#define CALLED(x...)		TRACE("CALLED %s\n", __PRETTY_FUNCTION__)

#define SDHCI_PCI_DEVICE_MODULE_NAME "busses/mmc/sdhci_pci/device/v1"

typedef struct {
	sdhci_sim sim;
	uint16 queue;
} sdhci_pci_queue_cookie;

typedef struct {
	pci_device_module_info* pci;
	pci_device* device;
	addr_t base_addr;
	uint8 irq;
	sdhci_irq_type irq_type;
	sdhci_sim sim;
	uint16 queue_count;

	device_node* node;
	pci_info info;

	sdhci_pci_queue_cookie *cookies;
} sdhci_pci_sim_info;

device_manager_info* gDeviceManager;

int32
sdhci_pci_queue_interrupt(void *data)
{
	sdhci_pci_queue_cookie* cookie = (sdhci_pci_queue_cookie*)data;
	gSDHCI->queue_interrupt_handler(cookie->sim, cookie->queue);
	return B_HANDLED_INTERRUPT;

}

int32
sdhci_pci_interrupt(void *data)
{
	sdhci_pci_sim_info* bus = (sdhci_pci_sim_info*)data;
	uint8 isr = bus->pci->read_io_8(bus->device,
		bus->base_addr) // incomplete
}

static void
bus_removed(void* bus_cookie)
{
	return;
}

static void
uninit_bus(void* bus_cookie) {}

static void 
init_bus(void *bus_init){}




static status_t
init_device(device_node* node, void** device_cookie)
{
	CALLED();
	*device_cookie = node;
	return B_OK;
}

static status_t
register_child_devices(void* cookie)
{
	CALLED();
	device_node* node = (device_node*)cookie;
	device_node* parent = gDeviceManager-> get_parent_node(node);
	pci_device_module_info *pci;
	pci_device* device; // incomplete 

}

static status_t
register_device(device_node* parent)
{
	device_attr attrs[] = {
		{B_DEVICE_PRETTY_NAME, B_STRING_TYPE, {string: "SDHCI PCI"}},
		{}
	};

	return gDeviceManager->register_node(parent, SDHCI_PCI_DEVICE_MODULE_NAME,
		attrs, NULL, NULL);
}



static float
supports_device(device_node* parent)
{
	CALLED();
	const char* bus;
	uint16 vendorID, deviceID;

	TRACE("Supports device started, first function has been loaded ")

	if (gDeviceManager->get_attr_string(parent, B_DEVICE_BUS, &bus, false) != B_OK
		|| gDeviceManager->get_attr_uint16(parent, B_DEVICE_VENDOR_ID,
				&vendorID, false) < B_OK
		|| gDeviceManager->get_attr_uint16(parent, B_DEVICE_ID, &deviceID,
				false) < B_OK) {
		return -1.0f;
	}

	if (strcmp(bus, "pci") != 0) 
		return 0.0f;

	/*if(get_attr_uint16(parent, B_DEVICE_TYPE, &deviceType, false) != 8 || get_attr_uint16(parent, B_DEVICE_SUBTYPE, &Subtype,false) != 5 || get_attr_uint16(parent, B_DEVICE_INTERFACE, &Interface, false) != 1)
		return 0; */// At the moment not needed, it won't be able to detect the specific fake vendor

	if (vendorID == SDHCI_PCI_VENDORID) { // check for vendor ID
		if (deviceID < SDHCI_PCI_MAX_DEVICEID 
			|| deviceID > SDHCI_PCI_MIN_DEVICEID) { // check for device ID
			return 0.0f;
		}

		pci_device_module_info* pci;
		pci_device* device;
		gDeviceManager->get_driver(parent, (driver_module_info**)&pci,
			(void**)&device);
		uint8 pciSubDeviceId = pci->read_pci_config(device, PCI_revision,
			1);
		// debug message 
		TRACE("SDHCI Device found! vendor 0x%04x, device 0x%04x", vendorID, deviceID);


		
		return 1.0f;
	}

	return 0;
}

static sdhci_sim_interface gSDHCIPCIDeviceModule = {
	{
		{
			SDHCI_PCI_SIM_MODULE_NAME,
			0,
			NULL
		},

		NULL,
		NULL,
		init_bus,
		uninit_bus,
		NULL,
		NULL,
		bus_removed
	}
}

module_dependency module_dependencies[] = {
//	{ SDHCI_FOR_CONTROLLER_MODULE_NAME, (module_info**)&gSDHCI },
	{ B_DEVICE_MANAGER_MODULE_NAME, (module_info**)&gDeviceManager },
	{}
};

static driver_module_info sSDHCIPCIDriver = {
	{
		SDHCI_PCI_DEVICE_MODULE_NAME,
		0,
		NULL
	},
	supports_device,
	register_device,
	init_device,
	NULL,
	register_child_devices,
	NULL,
	NULL
};

module_info* modules[] = { 
	(module_info* )&sSDHCIPCIDriver,
	NULL
};

