import SpcInfo from "components/SpcInfo";
import SpcReader from "SpcReader";

export default function SpcList({
  spcReaderList,
  playCallback,
}: {
  spcReaderList: {};
  playCallback: { (arg0: SpcReader): Promise<void> };
}) {
  return (
    <table>
      <thead>
        <tr>
          <th>Name</th>
          <th>Game</th>
          <th>Artist</th>
          <th>Comments</th>
          <th>Length</th>
          <th>Play</th>
        </tr>
      </thead>
      <tbody>
        {Object.entries(spcReaderList).map(([name, spcReader]) => (
          <SpcInfo
            key={name}
            spcReader={spcReader as SpcReader}
            playCallback={playCallback}
          />
        ))}
      </tbody>
    </table>
  );
}
