import { useState } from "react";

import FileHandler from "./components/FileHandler";
import SpcList from "./components/SpcList";

import SpcReader from "./SpcReader";

function App() {
  const [spcReaderList, setSpcReaderList] = useState({});

  async function fileCallback(file) {
    let buffer = await file.arrayBuffer();
    let spcReader = new SpcReader(file.name, buffer);
    setSpcReaderList((state) => ({ [spcReader.name()]: spcReader, ...state }));
  }

  return (
    <div>
      <div>
        <FileHandler fileCallback={fileCallback} />
      </div>
      <div>
        <SpcList spcReaderList={spcReaderList} />
      </div>
    </div>
  );
}

export default App;
