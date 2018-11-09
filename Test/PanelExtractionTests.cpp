#include "gtest/gtest.h"
#include "Panel.h"

class PanelExtractionTests : public testing::Test
{
protected:
	// Test constructor, does not attach to process
	Panel _panel = Panel();
	// std::shared_ptr<Panel> _panel;

	void SetPanelSize(int width, int height) {
		// _panel = std::make_shared<Panel>();
		_panel._width = width;
		_panel._height = height;
	}

	std::tuple<int, int> loc_to_xy(int location) {return _panel.loc_to_xy(location);}
	int xy_to_loc(int x, int y) {return _panel.xy_to_loc(x, y);}
	std::tuple<int, int> dloc_to_xy(int location) {return _panel.dloc_to_xy(location);}
	int xy_to_dloc(int x, int y) {return _panel.xy_to_dloc(x, y);}
};

TEST_F(PanelExtractionTests, 1x1) {
	SetPanelSize(1, 1);
	/* Here's the panel, with the correct location order
	0(_width = 1)
	(_height = 1)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 0));
	ASSERT_EQ((std::tuple<int, int>{0, 0}), loc_to_xy(0));
}

TEST_F(PanelExtractionTests, 7x1) {
	SetPanelSize(7, 1);
	/* Here's the panel, with the correct location order
	3 . 2 . 1 . 0(_width = 7)
	(_height = 1)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 0));
	ASSERT_EQ(1, xy_to_loc(2, 0));
	ASSERT_EQ(2, xy_to_loc(4, 0));
	ASSERT_EQ(3, xy_to_loc(6, 0));
	ASSERT_EQ((std::tuple<int, int>{0, 0}), loc_to_xy(0));
	ASSERT_EQ((std::tuple<int, int>{2, 0}), loc_to_xy(1));
	ASSERT_EQ((std::tuple<int, int>{4, 0}), loc_to_xy(2));
	ASSERT_EQ((std::tuple<int, int>{6, 0}), loc_to_xy(3));
}

TEST_F(PanelExtractionTests, 1x7) {
	SetPanelSize(1, 7);
	/* Here's the panel, with the correct location order
	3(_width = 1)
	.
	2
	.
	1
	.
	0
	(_height = 7)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 6));
	ASSERT_EQ(1, xy_to_loc(0, 4));
	ASSERT_EQ(2, xy_to_loc(0, 2));
	ASSERT_EQ(3, xy_to_loc(0, 0));
	ASSERT_EQ((std::tuple<int, int>{0, 6}), loc_to_xy(0));
	ASSERT_EQ((std::tuple<int, int>{0, 4}), loc_to_xy(1));
	ASSERT_EQ((std::tuple<int, int>{0, 2}), loc_to_xy(2));
	ASSERT_EQ((std::tuple<int, int>{0, 0}), loc_to_xy(3));
}

TEST_F(PanelExtractionTests, 3x3) {
	SetPanelSize(3, 3);
	/* Here's the panel, with the correct location order
	2 . 3 (_width = 3)
	. A .
	0 . 1
	(_height = 3)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 2));
	ASSERT_EQ(1, xy_to_loc(2, 2));
	ASSERT_EQ(2, xy_to_loc(0, 0));
	ASSERT_EQ(3, xy_to_loc(2, 0));
	ASSERT_EQ((std::tuple<int, int>{0, 2}), loc_to_xy(0));
	ASSERT_EQ((std::tuple<int, int>{2, 2}), loc_to_xy(1));
	ASSERT_EQ((std::tuple<int, int>{0, 0}), loc_to_xy(2));
	ASSERT_EQ((std::tuple<int, int>{2, 0}), loc_to_xy(3));

	ASSERT_EQ(0, xy_to_dloc(1, 1));
	ASSERT_EQ((std::tuple<int, int>{1, 1}), dloc_to_xy(0));
}

TEST_F(PanelExtractionTests, 7x5) {
	SetPanelSize(7, 5);
	/* Here's the panel, with the correct location order
	8 . 9 . 10. 11 (_width = 7)
	. D . E . F .
	4 . 5 . 6 . 7
	. A . B . C .
	0 . 1 . 2 . 3
	(_height = 5)
	*/
	ASSERT_EQ(0, xy_to_loc(0, 4));
	ASSERT_EQ(1, xy_to_loc(2, 4));
	ASSERT_EQ(2, xy_to_loc(4, 4));
	ASSERT_EQ(3, xy_to_loc(6, 4));
	ASSERT_EQ((std::tuple<int, int>{0, 4}), loc_to_xy(0));
	ASSERT_EQ((std::tuple<int, int>{2, 4}), loc_to_xy(1));
	ASSERT_EQ((std::tuple<int, int>{4, 4}), loc_to_xy(2));
	ASSERT_EQ((std::tuple<int, int>{6, 4}), loc_to_xy(3));

	ASSERT_EQ(4, xy_to_loc(0, 2));
	ASSERT_EQ(5, xy_to_loc(2, 2));
	ASSERT_EQ(6, xy_to_loc(4, 2));
	ASSERT_EQ(7, xy_to_loc(6, 2));
	ASSERT_EQ((std::tuple<int, int>{0, 2}), loc_to_xy(4));
	ASSERT_EQ((std::tuple<int, int>{2, 2}), loc_to_xy(5));
	ASSERT_EQ((std::tuple<int, int>{4, 2}), loc_to_xy(6));
	ASSERT_EQ((std::tuple<int, int>{6, 2}), loc_to_xy(7));

	ASSERT_EQ(8,  xy_to_loc(0, 0));
	ASSERT_EQ(9,  xy_to_loc(2, 0));
	ASSERT_EQ(10, xy_to_loc(4, 0));
	ASSERT_EQ(11, xy_to_loc(6, 0));
	ASSERT_EQ((std::tuple<int, int>{0, 0}), loc_to_xy(8));
	ASSERT_EQ((std::tuple<int, int>{2, 0}), loc_to_xy(9));
	ASSERT_EQ((std::tuple<int, int>{4, 0}), loc_to_xy(10));
	ASSERT_EQ((std::tuple<int, int>{6, 0}), loc_to_xy(11));
}
