import { useState } from "react";

import FileHandler from "./components/FileHandler";
import SpcList from "./components/SpcList";
import UsbConnect from "./components/UsbConnect";

import SpcReader from "./SpcReader";
import Serial from "./Serial";

export default function App() {
  const [spcReaderList, setSpcReaderList] = useState({});
  let serial: Serial | null = null;

  async function fileCallback(file: File) {
    let buffer = await file.arrayBuffer();
    let spcReader = new SpcReader(file.name, buffer);
    setSpcReaderList((state) => ({ [spcReader.name()]: spcReader, ...state }));
  }

  async function serialConnect(newSerial: Serial) {
    console.log("connect");
    serial = newSerial;
  }

  async function serialDisconnect() {
    console.log("disconnect");
  }

  async function reset() {
    if (serial) {
      let result = await serial.write(new TextEncoder().encode("R"));
      console.log(result);
      let data = await serial.read(1);
      console.log("aaa");
      console.log(new TextDecoder().decode(data.data));
    }
  }

  return (
    <div>
      <div>
        <UsbConnect
          connectCallback={serialConnect}
          disconnectCallback={serialDisconnect}
        />
      </div>
      <button onClick={reset}>Reset</button>
      <div>
        <FileHandler fileCallback={fileCallback} />
      </div>
      <div>
        <SpcList spcReaderList={spcReaderList} />
      </div>
    </div>
  );
}
