#pragma once
//
// CalendarView — pure UIKit view for the "calendar" module.
//
// Follows Aria demo3 pattern: View inherits UIView, is pure UI, exposes
// outlets for the Controller to bind. Does NOT know about CalendarVm or
// BindingEngine — that's the Controller's job.
//
#import <UIKit/UIKit.h>

@interface CalendarView : UIView

// Outlets the Controller binds to.
@property (nonatomic, strong, readonly) UILabel *titleLabel;
@property (nonatomic, strong, readonly) UILabel *hintLabel;
@property (nonatomic, strong, readonly) UILabel *statusLabel;
@property (nonatomic, strong, readonly) UILabel *monthTitleLabel;
@property (nonatomic, strong, readonly) UIButton *prevButton;
@property (nonatomic, strong, readonly) UIButton *nextButton;
@property (nonatomic, strong, readonly) UIButton *todayButton;
@property (nonatomic, strong, readonly) UIButton *refreshButton;
@property (nonatomic, strong, readonly) UITextField *urlField;
@property (nonatomic, strong, readonly) UIButton *subscribeButton;

@end
