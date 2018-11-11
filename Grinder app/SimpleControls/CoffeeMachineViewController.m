

#import "CoffeeMachineViewController.h"


@interface CoffeeMachineViewController ()


@end

@implementation CoffeeMachineViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSString *ver = [[UIDevice currentDevice] systemVersion];
    float ver_float = [ver floatValue];
    if(ver_float >= 9.0)  [self loadCoffeeMachineWKWebView];
    if(ver_float < 9.0) [self loadCoffeeMachineUIWebView];
    
}

- (IBAction)dot2:(id)sender {
    
   [self performSegueWithIdentifier:@"goToGrinder" sender:self];
    
}

- (void)loadCoffeeMachineUIWebView
{
    
    UIWebView *webview= [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, 320, 738)];
    [self webView].UIDelegate = self;

    self.productURL = @"http://coffee:1234@nickmcoyle.hopto.org";
    NSString *portNumber = @"9675";
    NSURL *aUrl = [NSURL URLWithString:[NSString stringWithFormat:@"%@:%@", self.productURL, portNumber]];
    NSURLRequest *nsrequest = [NSURLRequest requestWithURL:aUrl];
    [webview loadRequest:nsrequest];
    webview.frame = CGRectMake(self.view.frame.origin.x,self.view.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height - 60);
    [self.view addSubview:webview];
}

- (void)loadCoffeeMachineWKWebView
{
 
    WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
    WKWebView *webView = [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
    [self webView].UIDelegate = self;
    //self.productURL = @"http://coffee:1234@nickmcoyle.hopto.org";
    self.productURL = @"http://coffee:1234@nickmcoyle.hopto.org";
    NSString *portNumber = @"9675";
    NSURL *aUrl = [NSURL URLWithString:[NSString stringWithFormat:@"%@:%@", self.productURL, portNumber]];
    NSURLRequest *request = [NSURLRequest requestWithURL:aUrl];

    webView = [[WKWebView alloc] initWithFrame:self.view.frame];
    [webView loadRequest:request];
    webView.frame = CGRectMake(self.view.frame.origin.x,self.view.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height);
    [self.view addSubview:webView];
    
    

}

- (void)webView:(WKWebView *)webView runJavaScriptAlertPanelWithMessage:(NSString *)message initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(void))completionHandler
{
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil
                                                                   message:message
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"OK", nil) style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        completionHandler();
    }]];
    
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)webView:(WKWebView *)webView runJavaScriptConfirmPanelWithMessage:(NSString *)message initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(BOOL result))completionHandler
{
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil
                                                                   message:message
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"Cancel", nil) style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        completionHandler(NO);
    }]];
    
    [alert addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"OK", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        completionHandler(YES);
    }]];
    
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)webView:(WKWebView *)webView runJavaScriptTextInputPanelWithPrompt:(NSString *)prompt defaultText:(NSString *)defaultText initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(NSString *result))completionHandler
{
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil
                                                                   message:prompt
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addTextFieldWithConfigurationHandler:^(UITextField *textField) {
        textField.placeholder = prompt;
        textField.secureTextEntry = NO;
        textField.text = defaultText;
    }];
    
    [alert addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"Cancel", nil) style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        completionHandler(nil);
    }]];
    
    [alert addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"OK", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        completionHandler([alert.textFields.firstObject text]);
    }]];
    
    [self presentViewController:alert animated:YES completion:nil];
}




@end



