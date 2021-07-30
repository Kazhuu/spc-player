import Table from "@material-ui/core/Table";
import TableBody from "@material-ui/core/TableBody";
import TableCell from "@material-ui/core/TableCell";
import TableContainer from "@material-ui/core/TableContainer";
import TableHead from "@material-ui/core/TableHead";
import TableRow from "@material-ui/core/TableRow";
import LinearProgress from "@material-ui/core/LinearProgress";

import SpcReader from "SpcReader";

export default function SpcList({
  spcReaderList,
  playingSongName,
  showProgress,
  progress,
  playCallback,
}: {
  spcReaderList: { [key: string]: SpcReader };
  playingSongName: string;
  showProgress: boolean;
  progress: number;
  playCallback: {
    (arg0: SpcReader): Promise<void>;
  };
}) {
  return (
    <TableContainer>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Game</TableCell>
            <TableCell>Name</TableCell>
            <TableCell>Artist</TableCell>
            <TableCell>Comments</TableCell>
            <TableCell>Length</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {Object.keys(spcReaderList).map((name) =>
            playingSongName === name && showProgress ? (
              <TableRow key={name}>
                <TableCell colSpan={5}>
                  <LinearProgress variant="determinate" value={progress} />
                </TableCell>
              </TableRow>
            ) : spcReaderList[name].hasID666Metadata ? (
              <TableRow
                hover
                key={name}
                selected={playingSongName === name}
                onClick={() => playCallback(spcReaderList[name])}
              >
                <TableCell>{spcReaderList[name].metadata!.game}</TableCell>
                <TableCell>{name}</TableCell>
                <TableCell>{spcReaderList[name].metadata!.artist}</TableCell>
                <TableCell>{spcReaderList[name].metadata!.comments}</TableCell>
                <TableCell>{spcReaderList[name].metadata!.length}</TableCell>
              </TableRow>
            ) : (
              <TableRow
                hover
                key={name}
                selected={playingSongName === name}
                onClick={() => playCallback(spcReaderList[name])}
              >
                <TableCell></TableCell>
                <TableCell>{name}</TableCell>
                <TableCell></TableCell>
                <TableCell></TableCell>
                <TableCell></TableCell>
              </TableRow>
            )
          )}
        </TableBody>
      </Table>
    </TableContainer>
  );
}
