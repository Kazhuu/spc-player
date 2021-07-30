import { useRef, SyntheticEvent } from "react";
import Button from "@material-ui/core/Button";

export default function FileHandler({
  fileCallback,
}: {
  fileCallback: { (arg0: File[]): void };
}) {
  const fileInput = useRef<HTMLInputElement>(null);

  const handleFileInput = (event: SyntheticEvent) => {
    event.preventDefault();
    if (
      fileInput !== null &&
      fileInput.current !== null &&
      fileInput.current.files !== null
    ) {
      let files = [];
      for (let i = 0; i < fileInput.current.files.length; i++) {
        let file = fileInput.current.files.item(i);
        if (file) {
          files.push(file);
        }
      }
      fileCallback(files);
    }
  };

  return (
    <Button
      variant="contained"
      color="primary"
      component="label"
      disableElevation
    >
      Choose Files
      <input
        type="file"
        onChange={handleFileInput}
        ref={fileInput}
        hidden
        multiple
      />
    </Button>
  );
}
