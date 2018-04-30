#include <new>
#include <stdio.h>
#include <string.h>

#include <bus/PCI.h>
#include <device_manager.h>

#include "sdhci_pci.h"


#define SDHCI_PCI_DEVICE_MODULE_NAME "busses/mmc/sdhci_pci/driver_v1"


device_manager_info* gDeviceManager;

static int
supports_device(device_node* parent)
{
	CALLED();
	const char* bus;
	uint16 vendorID, deviceID;

	if (gDeviceManager->get_attr_string(parent, B_DEVICE_BUS, &bus, false) != B_OK
		|| gDeviceManager->get_attr_uint16(parent, B_DEVICE_VENDOR_ID,
				&vendorID, false) < B_OK
		|| gDeviceManager->get_attr_uint16(parent, B_DEVICE_ID, &deviceID,
				false) < B_OK) {
		return -1;
	}

	if (strcmp(bus, "pci") != 0) 
		return 0;

	/*if(get_attr_uint16(parent, B_DEVICE_TYPE, &deviceType, false) != 8 || get_attr_uint16(parent, B_DEVICE_SUBTYPE, &Subtype,false) != 5 || get_attr_uint16(parent, B_DEVICE_INTERFACE, &Interface, false) != 1)
		return 0; */// At the moment not needed, it won't be able to detect the specific fake vendor

	if (vendorID == SDHCI_PCI_VENDORID) { // check for vendor ID
		if (deviceID < SDHCI_PCI_MAX_DEVICEID 
			|| deviceID > SDHCI_PCI_MIN_DEVICEID) { // check for device ID
			return 0;
		}

		pci_device_module_info* pci;
		pci_device* device;
		gDeviceManager->get_driver(parent, (driver_module_info**)&pci,
			(void**)&device);
		uint8 pciSubDeviceId = pci->read_pci_config(device, PCI_revision,
			1);
		// debug message 
		dprintf("SDHCI Device found! vendor 0x%04x, device 0x%04x", vendorID, deviceID);


		
		return 1;
	}

	return 0;
}

static driver_module_info sSDHCIPCIDriver = {
	{
		SDHCI_PCI_DEVICE_MODULE_NAME,
		0,
		NULL
	},
	supports_device,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

module_info* modules[] = { 
	(module_info* )&sSDHCIPCIDriver,
	NULL
};

