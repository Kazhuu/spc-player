export default class Serial {
  private device: USBDevice;
  private interfaceNumber: number;
  private endpointIn: number;
  private endpointOut: number;

  constructor(device: USBDevice) {
    this.device = device;
  }

  public async connect() {
    await this.device.open();
    if (this.device.configuration === null) {
      await this.device.selectConfiguration(1);
    }
    let configurationInterfaces = this.device.configuration!.interfaces;
    configurationInterfaces.forEach((element) => {
      element.alternates.forEach((elementalt) => {
        if (elementalt.interfaceClass === 0xff) {
          this.interfaceNumber = element.interfaceNumber;
          elementalt.endpoints.forEach((elementendpoint) => {
            if (elementendpoint.direction === "out") {
              this.endpointOut = elementendpoint.endpointNumber;
            }
            if (elementendpoint.direction === "in") {
              this.endpointIn = elementendpoint.endpointNumber;
            }
          });
        }
      });
    });
    await this.device.claimInterface(this.interfaceNumber);
    await this.device.selectAlternateInterface(this.interfaceNumber, 0);
    await this.device.controlTransferOut({
      requestType: "class",
      recipient: "interface",
      request: 0x22,
      value: 0x01,
      index: this.interfaceNumber,
    });
  }

  public write(data: BufferSource): Promise<USBOutTransferResult> {
    return this.device.transferOut(this.endpointOut, data);
  }

  public read(size: number): Promise<USBInTransferResult> {
    return this.device.transferIn(this.endpointIn, size);
  }

  public async disconnect(): Promise<void> {
    await this.device.controlTransferOut({
      requestType: "class",
      recipient: "interface",
      request: 0x22,
      value: 0x00,
      index: this.interfaceNumber,
    });
    return await this.device.close();
  }
}
