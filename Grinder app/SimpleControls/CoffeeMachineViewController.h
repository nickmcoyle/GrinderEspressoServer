

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

@interface CoffeeMachineViewController : UIViewController <WKUIDelegate>{
    
}
@property (strong, nonatomic) IBOutlet UIPageControl *dot2;

@property(strong,nonatomic) WKWebView *webView;
@property(strong,nonatomic) UIWebView *uiwebView;
@property (strong, nonatomic) NSString *productURL;




@end
