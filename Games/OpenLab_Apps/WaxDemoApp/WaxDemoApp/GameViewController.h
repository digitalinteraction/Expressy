//
//  GameViewController.h
//  WaxDemoApp
//
//  Created by localadmin on 31/12/2015.
//  Copyright © 2015 localadmin. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <Corebluetooth/CoreBluetooth.h>

@interface GameViewController : GLKViewController
<
CBCentralManagerDelegate,
CBPeripheralDelegate
>

@property (nonatomic, strong) CBCentralManager          *centralManager;
@property (nonatomic, strong) CBPeripheral              *wax9Peripheral;

@end
