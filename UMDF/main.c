// main.c : the application source code.
#include "main.h"
#include <windows.h>
#include <wdf.h>
#include <string.h>


EVT_WDF_DRIVER_DEVICE_ADD EvtWdfDriverDeviceAdd;

// 1. the first driver-supplied routine that is called after a driver is loaded
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS status = STATUS_SUCCESS;
    
    WDF_DRIVER_CONFIG config;
    WDF_DRIVER_CONFIG_INIT(
        &config,
        EvtWdfDriverDeviceAdd
    );

    // 2. enables the driver to use Windows Driver Framework interfaces
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );

    return status;
};

// 3. performs device initialization operations when the Plug and Play (PnP) manager reports the existence of a device
NTSTATUS EvtWdfDriverDeviceAdd(
    _In_    WDFDRIVER       Driver, 
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Driver);

    /* framework device object initialization methods
    ...
    */

    // 4. identifies the calling driver as an upper-level or lower-level filter driver, for a specified device
    WdfFdoInitSetFilter(
        DeviceInit
    );
    
    WDFDEVICE device;

    // 5. creates a framework device object
    status = WdfDeviceCreate(
        &DeviceInit, 
        WDF_NO_OBJECT_ATTRIBUTES,
        &device
    );

    WDF_IO_QUEUE_CONFIG config;
    config.DispatchType = WdfIoQueueDispatchSequential;
    config.PowerManaged = WdfFalse;

    WDFQUEUE queue;

    // 6. creates and configures an I/O queue for a specified device
    status = WdfIoQueueCreate(
        device,
        &config,
        WDF_NO_OBJECT_ATTRIBUTES,
        &queue
    );

    return status;
}

EVT_WDF_IO_QUEUE_IO_DEFAULT EvtWdfIoQueueIoDefault;

// 7. processes a specified I/O request
void EvtWdfIoQueueIoDefault(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request
)
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Queue);

    // console output
    HWND hWnd = FindWindowA(
        NULL,
        "Администратор: Командная строка"
    );
    if (hWnd != NULL)
    {
        char* msg = "WORK";
        for (int i = 0; i < strlen(msg); i++)
        {
            PostMessageA(
                hWnd,
                WM_CHAR,
                (int)msg[i],
                0
            );
        }
        int wparam = 0 << 29 | 0;
        PostMessageA(
            hWnd,
            WM_KEYDOWN,
            *"/n",
            wparam
        );
    }
    // console output

    // 8. completes a specified I/O request and supplies a completion status
    WdfRequestComplete(
        Request,
        status
    );
}

/*
#include <ntddk.h>
#include <wdf.h>

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KmdfHelloWorldEvtDeviceAdd;

NTSTATUS 
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject, 
    _In_ PUNICODE_STRING    RegistryPath
)
{
    // NTSTATUS variable to record success or failure
    NTSTATUS status = STATUS_SUCCESS;

    // Allocate the driver configuration object
    WDF_DRIVER_CONFIG config;

    // Print "Hello World" for DriverEntry
    KdPrintEx(( DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: DriverEntry\n" ));

    // Initialize the driver configuration object to register the
    // entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
    WDF_DRIVER_CONFIG_INIT(&config, 
                           KmdfHelloWorldEvtDeviceAdd
                           );

    // Finally, create the driver object
    status = WdfDriverCreate(DriverObject, 
                             RegistryPath, 
                             WDF_NO_OBJECT_ATTRIBUTES, 
                             &config, 
                             WDF_NO_HANDLE
                             );
    return status;
}

NTSTATUS 
KmdfHelloWorldEvtDeviceAdd(
    _In_    WDFDRIVER       Driver, 
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    // We're not using the driver object,
    // so we need to mark it as unreferenced
    UNREFERENCED_PARAMETER(Driver);

    NTSTATUS status;

    // Allocate the device object
    WDFDEVICE hDevice;    

    // Print "Hello World"
    KdPrintEx(( DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n" ));

    // Create the device object
    status = WdfDeviceCreate(&DeviceInit, 
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &hDevice
                             );
    return status;
}
*/