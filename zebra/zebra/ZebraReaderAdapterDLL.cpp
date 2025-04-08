#include "ZebraReaderAdapterDLL.h"
#include "ZebraReaderAdapter.h"

extern "C" {

    ZEBRA_ADAPTER_API IRFIDReader* CreateRFIDReader(void* server)
    {
		printf("\nIRFIDReader.CreateRFIDReader v4.0\n");
        return new ZebraReaderAdapter(server);
    }

    ZEBRA_ADAPTER_API void DestroyRFIDReader(IRFIDReader* reader)
    {
        delete reader;
    }

	//`機種情報
	ZEBRA_ADAPTER_API DeviceInterface* get_device_interface(void) {
		static  DeviceInterface dev;
		memset(&dev, 0, sizeof(DeviceInterface));
		dev.manufacturer = "Zebra Technologies";
		dev.model_name = "FX9600";
		dev.protocol_version = "3.0"; //版本
		printf("DeviceInterface[zebra.dll] v3.0\n");




		return &dev;

	}


} // extern "C"
