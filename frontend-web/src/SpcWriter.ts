import Serial from "Serial";

export default class SpcWriter {
  private serial: Serial;
  private encoder: TextEncoder;
  private decoder: TextDecoder;

  public constructor(serial: Serial) {
    this.serial = serial;
    this.encoder = new TextEncoder();
    this.decoder = new TextDecoder();
  }

  public async writeCpuRegisters(
    programCounter: number,
    a: number,
    x: number,
    y: number,
    stackPointer: number,
    programStatusWord: number
  ) {
    await this.serial.write(this.encode("C"));
    await this.serial.write(this.numberToByte(programCounter));
    await this.serial.write(this.numberToByte(a));
    await this.serial.write(this.numberToByte(x));
    await this.serial.write(this.numberToByte(y));
    await this.serial.write(this.numberToByte(stackPointer));
    await this.serial.write(this.numberToByte(programStatusWord));
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) === "1") {
      throw new Error("writing CPU registers failed");
    }
  }

  public writeDspRegisters(dspRegisters: Uint8Array) {}
  public writeRirstPageRam(firstPageRam: Uint8Array) {}
  public writeSecondPageRam(secondPageRam: Uint8Array) {}
  public writeRestOfTheRam(restOfTheRam: Uint8Array) {}
  public readPorts(): Uint8Array {
    return new Uint8Array();
  }
  public writePort(port: number, value: number) {}
  public start() {}
  public async reset() {
    await this.serial.write(this.encode("R"));
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) === "1") {
      console.log("reset");
    } else {
      throw new Error("SPC reset timed out");
    }
  }

  private encode(data: string) {
    return this.encoder.encode(data);
  }

  private decode(data: ArrayBuffer) {
    return this.decoder.decode(data);
  }

  private numberToByte(number: number) {
    let arr = new ArrayBuffer(1);
    let view = new DataView(arr);
    view.setUint8(0, number);
    return arr;
  }

  private numberToShort(number: number) {
    let arr = new ArrayBuffer(2);
    let view = new DataView(arr);
    view.setUint16(0, number);
    return arr;
  }
}
