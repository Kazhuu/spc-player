import { useState, useEffect } from "react";

export default function UsbConnect(props: any) {
  const [deviceFound, setDeviceFound] = useState(false);

  useEffect(() => {
    async function getDevice() {
      let devices = await navigator.usb.getDevices();
      if (devices.length > 0) {
        setDeviceFound(true);
        props.connectCallback(devices[0]);
      } else {
        setDeviceFound(false);
      }
    }
    getDevice();
  }, []);

  async function connect() {
    if (!deviceFound) {
      const filters = [{ vendorId: 0x2341 }]; // Arduino vendor id
      let device = await navigator.usb.requestDevice({ filters: filters });
      setDeviceFound(true);
      props.connectCallback(device);
    } else {
      props.disconnectCallback();
      setDeviceFound(false);
    }
  }

  return (
    <div>
      <button onClick={connect}>
        {deviceFound ? "Disconnect" : "Connect"}
      </button>
      <p>Status: {deviceFound ? "Device found" : "Device not found"}</p>
    </div>
  );
}
