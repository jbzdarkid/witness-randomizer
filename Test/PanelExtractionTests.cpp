#include "gtest/gtest.h"
#include "Panel.h"

class PanelExtractionTests : public testing::Test
{
protected:
	Panel _panel;
	// std::shared_ptr<Panel> _panel;

	void SetPanelSize(int width, int height) {
		// _panel = std::make_shared<Panel>();
		_panel._width = width;
		_panel._height = height;
	}

	std::tuple<int, int> loc_to_xy(int location) {
		return _panel.loc_to_xy(location);
	}

	int xy_to_loc(int x, int y) {
		return _panel.xy_to_loc(x, y);
	}
};

TEST_F(PanelExtractionTests, LocToXY_7x5) {
	SetPanelSize(7, 5);
	/* Here's the panel, with the correct location order
	8 . 9 . 10. 11 (_width = 7)
	. . . . . . .
	4 . 5 . 6 . 7
	. . . . . . .
	0 . 1 . 2 . 3
	(_height = 5)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 4));
	ASSERT_EQ(1, xy_to_loc(2, 4));
	ASSERT_EQ(2, xy_to_loc(4, 4));
	ASSERT_EQ(3, xy_to_loc(6, 4));

	ASSERT_EQ(4, xy_to_loc(0, 2));
	ASSERT_EQ(5, xy_to_loc(2, 2));
	ASSERT_EQ(6, xy_to_loc(4, 2));
	ASSERT_EQ(7, xy_to_loc(6, 2));

	ASSERT_EQ(8, xy_to_loc(0, 0));
	ASSERT_EQ(9, xy_to_loc(2, 0));
	ASSERT_EQ(10, xy_to_loc(4, 0));
	ASSERT_EQ(11, xy_to_loc(6, 0));
}

TEST_F(PanelExtractionTests, LocToXY_3x3) {
	SetPanelSize(3, 3);
	/* Here's the panel, with the correct location order
	2 . 3 (_width = 3)
	. . .
	0 . 1
	(_height = 3)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 2));
	ASSERT_EQ(1, xy_to_loc(2, 2));
	ASSERT_EQ(2, xy_to_loc(0, 0));
	ASSERT_EQ(3, xy_to_loc(2, 0));
}