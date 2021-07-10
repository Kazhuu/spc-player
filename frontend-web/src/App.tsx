import { useState } from "react";

import FileHandler from "./components/FileHandler";
import SpcList from "./components/SpcList";
import UsbConnect from "./components/UsbConnect";

import SpcReader from "./SpcReader";
import Serial from "./Serial";

export default function App() {
  const [spcReaderList, setSpcReaderList] = useState({});

  async function fileCallback(file: File) {
    let buffer = await file.arrayBuffer();
    let spcReader = new SpcReader(file.name, buffer);
    setSpcReaderList((state) => ({ [spcReader.name()]: spcReader, ...state }));
  }

  async function serialConnect(serial: Serial) {
    console.log("connect");
    //console.log(serial);
    //let result = await serial.write(new TextEncoder().encode("RRRRRRRRRRR"));
    //console.log(result);
    let data = await serial.read(64);
    console.log("aaa");
    console.log(data);
  }

  async function serialDisconnect() {
    console.log("disconnect");
  }

  return (
    <div>
      <div>
        <UsbConnect
          connectCallback={serialConnect}
          disconnectCallback={serialDisconnect}
        />
      </div>
      <div>
        <FileHandler fileCallback={fileCallback} />
      </div>
      <div>
        <SpcList spcReaderList={spcReaderList} />
      </div>
    </div>
  );
}
