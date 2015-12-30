#include <string>
#import <Foundation/Foundation.h>

@interface NSString (CXXAdditions)
+ (NSString*)stringWithStdString:(const std::string&)str;
- (NSString*)initWithStdString:(const std::string&)str;
@end
@implementation NSString (CXXAdditions)
+ (NSString*)stringWithStdString:(const std::string&)str {
	return [self stringWithUTF8String:str.c_str()];
}
- (NSString*)initWithStdString:(const std::string&)str {
	return [self initWithUTF8String:str.c_str()];
}
@end
