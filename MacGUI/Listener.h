//
//  Listener.h
//  V64
//
//  Created by Dirk Hoffmann on 07.04.08.

#import "MyDocument.h"


@interface MyDocument (Listener) <ListenerProtocol>

- (void) runAction;
- (void) haltAction;

@end
