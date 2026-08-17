#import "CalendarView.h"

@implementation CalendarView

- (instancetype)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        [self setupSubviews];
    }
    return self;
}

- (void)setupSubviews {
    // Create all outlets (pure UI — no VM, no bindings).
    _titleLabel = [[UILabel alloc] init];
    _titleLabel.font = [UIFont boldSystemFontOfSize:22];
    _titleLabel.numberOfLines = 0;

    _hintLabel = [[UILabel alloc] init];
    _hintLabel.font = [UIFont systemFontOfSize:12];
    _hintLabel.numberOfLines = 0;
    _hintLabel.textColor = [UIColor secondaryLabelColor];

    _monthTitleLabel = [[UILabel alloc] init];
    _monthTitleLabel.font = [UIFont boldSystemFontOfSize:16];
    _monthTitleLabel.textAlignment = NSTextAlignmentCenter;

    _prevButton = [UIButton buttonWithType:UIButtonTypeSystem];
    _nextButton = [UIButton buttonWithType:UIButtonTypeSystem];
    _todayButton = [UIButton buttonWithType:UIButtonTypeSystem];
    _refreshButton = [UIButton buttonWithType:UIButtonTypeSystem];

    _urlField = [[UITextField alloc] init];
    _urlField.borderStyle = UITextBorderStyleRoundedRect;

    _subscribeButton = [UIButton buttonWithType:UIButtonTypeSystem];

    _statusLabel = [[UILabel alloc] init];
    _statusLabel.font = [UIFont systemFontOfSize:12];
    _statusLabel.numberOfLines = 0;

    // Nav row (horizontal stack).
    UIStackView *navRow = [[UIStackView alloc] initWithArrangedSubviews:@[
        _prevButton, _monthTitleLabel, _nextButton, _todayButton, _refreshButton
    ]];
    navRow.axis = UILayoutConstraintAxisHorizontal;
    navRow.spacing = 8;
    navRow.distribution = UIStackViewDistributionFill;

    // Sub row (URL + subscribe).
    UIStackView *subRow = [[UIStackView alloc] initWithArrangedSubviews:@[
        _urlField, _subscribeButton
    ]];
    subRow.axis = UILayoutConstraintAxisHorizontal;
    subRow.spacing = 8;
    subRow.distribution = UIStackViewDistributionFill;

    // Main vertical stack.
    UIStackView *stack = [[UIStackView alloc] initWithArrangedSubviews:@[
        _titleLabel, _hintLabel, navRow, subRow, _statusLabel
    ]];
    stack.axis = UILayoutConstraintAxisVertical;
    stack.spacing = 12;
    stack.alignment = UIStackViewAlignmentFill;
    stack.translatesAutoresizingMaskIntoConstraints = NO;

    [self addSubview:stack];
    [NSLayoutConstraint activateConstraints:@[
        [stack.topAnchor constraintEqualToAnchor:self.safeAreaLayoutGuide.topAnchor constant:16],
        [stack.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:16],
        [stack.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-16],
        [stack.bottomAnchor constraintLessThanOrEqualToAnchor:self.safeAreaLayoutGuide.bottomAnchor constant:-16],
    ]];
}

@end
