import { useState } from "react";

import FileHandler from "./components/FileHandler";
import SpcInfo from "./components/SpcInfo";

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
      <FileHandler fileCallback={fileCallback} />
      {Object.entries(spcReaderList).map(([name, spcReader]) => (
        <SpcInfo key={name} spcReader={spcReader} />
      ))}
    </div>
  );
}

export default App;
