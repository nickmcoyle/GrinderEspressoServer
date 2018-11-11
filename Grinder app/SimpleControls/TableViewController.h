

#import <UIKit/UIKit.h>
#import "BLE.h"

@interface TableViewController : UITableViewController <BLEDelegate>
{
    
    IBOutlet UIButton *btnConnect;
    IBOutlet UISwitch *btnTare;
    IBOutlet UISwitch *btnGrind;
    IBOutlet UIActivityIndicatorView *indConnecting;
    IBOutlet UILabel *lblRSSI;
    IBOutlet UILabel *lblLoadLimit;
    IBOutlet UIStepper *stepperLoadLimit;
    IBOutlet UIStepper *lagFactorSlider;
    __weak IBOutlet UISlider *LagFactorSlider;
    IBOutlet UILabel *lblLagFactor;
    IBOutlet UILabel *lblLoad;
}
@property (strong, nonatomic) IBOutlet UIPageControl *dot1;

@property (strong, nonatomic) BLE *ble;


@end
