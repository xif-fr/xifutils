#import <Foundation/Foundation.h>

inline void block_in_main(void(^block)(void)) {
	[[NSOperationQueue mainQueue] addOperationWithBlock:block];
}

inline void NSLogVar (id obj) {
	NSLog(@"%@", obj);
}
