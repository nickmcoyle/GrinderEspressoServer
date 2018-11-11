

#import "TableViewController.h"

@interface TableViewController ()

@end

@implementation TableViewController

@synthesize ble;

int oldValue;


- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
    
    
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    ble = [[BLE alloc] init];
    [ble controlSetup];
    ble.delegate = self;
    
    self->stepperLoadLimit.wraps = YES; self->stepperLoadLimit.autorepeat = YES;
    
  }

- (IBAction)dot1:(id)sender {
    
    [self performSegueWithIdentifier:@"goToCoffeeMachine" sender:self];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - BLE delegate

NSTimer *rssiTimer;

- (void)bleDidDisconnect
{
    NSLog(@"->Disconnected");

    [btnConnect setTitle:@"Connect" forState:UIControlStateNormal];
    [indConnecting stopAnimating];
    
    btnGrind.enabled = false;
    btnTare.enabled = false;
        
    //lblRSSI.text = @"---";
    
    lblLoad.text = @"";
    lblLoadLimit.text = @"";
    
    [rssiTimer invalidate];
}

// When RSSI is changed, this will be called
-(void) bleDidUpdateRSSI:(NSNumber *) rssi
{
    lblRSSI.text = rssi.stringValue;
   // lblRSSI.text = [NSString stringWithFormat:@"%d",[[CBPeripheral RSSI] intValue]];
}

-(void) readRSSITimer:(NSTimer *)timer
{
    [ble readRSSI];
}

// When connected, this will be called
-(void) bleDidConnect
{
    NSLog(@"->Connected");

    [indConnecting stopAnimating];
    
   
    btnTare.enabled = true;
    btnGrind.enabled = true;
    btnTare.on = false;
    btnGrind.on = false;

    
    // send reset
    UInt8 buf[] = {0x04, 0x00, 0x00};
    NSData *data = [[NSData alloc] initWithBytes:buf length:3];
    [ble write:data];

    // Schedule to read RSSI every 1 sec.
    rssiTimer = [NSTimer scheduledTimerWithTimeInterval:(float)1.0 target:self selector:@selector(readRSSITimer:) userInfo:nil repeats:YES];
}

// When data is comming, this will be called
-(void) bleDidReceiveData:(signed char *)data length:(int)len
{
    
NSLog(@"Load: %s  Limit: %d", data, len);
    NSString *load;
    UInt16 loadLimit;
    
    load = [NSString stringWithFormat:@"%s", data];
    self->lblLoad.text = [NSString stringWithFormat:@"%@ g",load];
    loadLimit = len;
    self->lblLoadLimit.text = [NSString stringWithFormat:@"%.0hu g", loadLimit];
    oldValue = loadLimit;
    self->stepperLoadLimit.value = loadLimit;
    
    // parse data, all commands are in 3-byte
  //  for (int i = 0; i < length; i+=3)
//{
    
       //NSLog(@"0x%02X, 0x%02X, 0x%02X", data[i], data[i+1], data[i+2]);

//    UInt16 loadLimit;
//    float lagFactor;
//    loadLimit = data[i+2];
//    lagFactor = data[i+1];
//    
    
  // NSLog(@"%d d0, %d d1, %d d2", data[i], data[i+1], data[i+2]);

//
//            self->lblLoadLimit.text = [NSString stringWithFormat:@"%.0hu g", loadLimit];
//            oldValue = loadLimit;
//            self->stepperLoadLimit.value = loadLimit;
            //self->lblLagFactor.text = [NSString stringWithFormat:@"%.2f g", lagFactor];
            //self->lagFactorSlider.value = lagFactor;
    
    
   // }
}

#pragma mark - Actions

// Connect button will call to this
- (IBAction)btnScanForPeripherals:(id)sender
{
    if (ble.activePeripheral)
        if(ble.activePeripheral.state == CBPeripheralStateConnected)
        {
            [[ble CM] cancelPeripheralConnection:[ble activePeripheral]];
            [btnConnect setTitle:@"Connect" forState:UIControlStateNormal];
            return;
        }
    
    if (ble.peripherals)
        ble.peripherals = nil;
    
    [btnConnect setEnabled:false];
    [ble findBLEPeripherals:2];
    
    [NSTimer scheduledTimerWithTimeInterval:(float)2.0 target:self selector:@selector(connectionTimer:) userInfo:nil repeats:NO];
    
    [indConnecting startAnimating];
}

-(void) connectionTimer:(NSTimer *)timer
{
    [btnConnect setEnabled:true];
    [btnConnect setTitle:@"Disconnect" forState:UIControlStateNormal];
    
    if (ble.peripherals.count > 0)
    {
        [ble connectPeripheral:[ble.peripherals objectAtIndex:0]];
    }
    else
    {
        [btnConnect setTitle:@"Connect" forState:UIControlStateNormal];
        [indConnecting stopAnimating];
    }
}

-(IBAction)sendTareCmd:(id)sender
{
    
    NSString *text = @"t";
    NSData *d;
    d = [text dataUsingEncoding:NSUTF8StringEncoding];
    NSLog(@"Data: %@", d);
    [ble write:d];

   
}

-(IBAction)sendGrindCmd:(id)sender
{
    NSString *text = @"g";
    NSData *d;
    d = [text dataUsingEncoding:NSUTF8StringEncoding];
    NSLog(@"Data: %@", d);
        [ble write:d];
    
}

//-(IBAction)changeLoadLimitCmd:(id)sender
- (IBAction)stepperValueChanged:(UIStepper *)sender;
{
    NSUInteger value = sender.value;
    self->lblLoadLimit.text = [NSString stringWithFormat:@"%02lu", (unsigned long)value];

    NSString *text;
    NSData *d;
    
    
    if (stepperLoadLimit.value > oldValue) {
        oldValue = oldValue+1;
        //command to send to arduino
         text = @"+";
    }
    else {
        oldValue=oldValue-1;
        //command to send to arduino
        text = @"-";
    }
    
    d = [text dataUsingEncoding:NSUTF8StringEncoding];
    [ble write:d];
    
}

- (IBAction)lagFactorValueChanged:(UISlider *)sender;
{
    
    float value = sender.value;
    NSString *text = [NSString stringWithFormat:@"%.2f", value];
    self->lblLagFactor.text = text;
    NSData *d;
    d = [text dataUsingEncoding:NSUTF8StringEncoding];
    NSLog(@"Data: %@", d);
  //  [ble write:d];
}





@end
