export enum Emulator {
  Unknown = 0,
  ZSNES,
  Snes9x,
}

export interface IId666 {
  // Song title
  title: string;
  // Game title
  game: string;
  // Name of SPC dumper
  dumpedBy: string;
  // SPC comments
  comments: string;
  // Date SPC was dumped
  dumpedOn: Date;
  // Length of song in seconds before fadeout
  length: number;
  // Length of fadeout in milliseconds
  fadeOutLength: number;
  // Song artist
  artist: string;
  // "Default channel disables" (?)
  defaultChannelDisabled: boolean;
  // Emulator used to dump SPC
  emulator: Emulator;
}

const SPC_HEADER = "SNES-SPC700 Sound File Data v0.30";

export default class SpcReader {
  public filename: string;
  public programCounter: Uint8Array;
  public a: number;
  public x: number;
  public y: number;
  public programStatusWord: number;
  public stackPointer: number;
  public hasID666Metadata: boolean;
  public metadata?: IId666;
  public dspRegisters: Uint8Array;
  public ram: Uint8Array;
  public firstPageRam: Uint8Array;
  public secondPageRam: Uint8Array;
  public restOfTheRam: Uint8Array;

  private buffer: Uint8Array;

  constructor(filename: string, spcFileBuffer: ArrayBuffer) {
    this.filename = filename;
    this.buffer = new Uint8Array(spcFileBuffer);
    this.parse();
  }

  name(): string {
    if (this.hasID666Metadata) {
      return this.metadata!.title;
    } else {
      return this.filename;
    }
  }

  private parse(): void {
    if (!this.verify()) {
      throw new Error("Not a valid SPC file");
    }

    this.parseSPCInfo();
    if (this.hasID666Metadata) {
      this.parseID666();
    }
    this.dspRegisters = this.buffer.slice(0x10100, 0x10180);
    this.ram = this.buffer.slice(0x100, 0x10100); // 65 536 bytes
    this.firstPageRam = this.ram.slice(0x0, 0x100); // 256 bytes
    this.secondPageRam = this.buffer.slice(0x0100, 0x0200); // 256 bytes
    this.restOfTheRam = this.buffer.slice(0x0200, 0xffc0); // 64 960 bytes
  }

  private verify(): boolean {
    return (
      this.bufferToString(0, 33) === SPC_HEADER &&
      this.buffer[0x21] === 26 &&
      this.buffer[0x22] === 26
    );
  }

  private parseSPCInfo(): void {
    this.hasID666Metadata = this.buffer[0x23] === 0x1a;
    this.a = this.buffer[0x27];
    this.x = this.buffer[0x28];
    this.y = this.buffer[0x29];
    this.programStatusWord = this.buffer[0x2a];
    this.stackPointer = this.buffer[0x2b];
    this.programCounter = this.buffer.slice(0x25, 0x25 + 2);
  }

  private parseID666(): void {
    this.metadata = {
      title: this.bufferToString(0x2e, 32),
      game: this.bufferToString(0x4e, 32),
      dumpedBy: this.bufferToString(0x6e, 16),
      comments: this.bufferToString(0x7e, 32),
      dumpedOn: new Date(this.bufferToString(0x9e, 11)),
      length: Number(this.bufferToString(0xa9, 3)),
      fadeOutLength: Number(this.bufferToString(0xac, 4)),
      artist: this.bufferToString(0xb1, 32),
      defaultChannelDisabled: this.buffer[0xd1] === 1,
      emulator: Number(this.bufferToString(0xd2, 1)),
    };
  }

  private bufferToString(start: number, length: number): string {
    return new TextDecoder("utf-8")
      .decode(this.buffer.slice(start, start + length))
      .replace(/\0/g, "");
  }
}
