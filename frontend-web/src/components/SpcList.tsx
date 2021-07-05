import SpcInfo from "../components/SpcInfo";
import SpcReader from "../SpcReader";

export default function SpcList({
  spcReaderList,
}: {
  spcReaderList: [SpcReader];
}) {
  return (
    <table>
      <tr>
        <th>Name</th>
        <th>Game</th>
        <th>Artist</th>
        <th>Comments</th>
        <th>Length</th>
      </tr>
      {Object.entries(spcReaderList).map(([name, spcReader]) => (
        <SpcInfo key={name} spcReader={spcReader} />
      ))}
    </table>
  );
}
