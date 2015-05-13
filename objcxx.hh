#include <string>
#import <Foundation/Foundation.h>

@interface NSString (CPPAdditions)
+ (NSString*)stringWithCPPString:(const std::string&)str;
- (NSString*)initWithCPPString:(const std::string&)str;
@end
@implementation NSString (CPPAdditions)
+ (NSString*)stringWithCPPString:(const std::string&)str {
	return [self stringWithUTF8String:str.c_str()];
}
- (NSString*)initWithCPPString:(const std::string&)str {
	return [self initWithUTF8String:str.c_str()];
}
@end
