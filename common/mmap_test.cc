#include "common/mmap.hh"
#include "common/file_descriptor.hh"
#include "testing/test.hh"
#include "testing/test_support.hh"

class MmapTest : public TestFixture {
 public:
  void Setup() override {
    test_file_ = vt::testing::get_bazel_test_dir_unique() + "/test_file";

    vt::common::FileDescriptor fd(test_file_.c_str(), O_WRONLY | O_CREAT,
                                  S_IRUSR | S_IWUSR);
    CHECK_TRUE(fd.valid());
    CHECK_NE(::write(fd.handle(), "abc", 3), -1);
  }
  void TearDown() override { CHECK_NE(::unlink(test_file_.c_str()), -1); }

 protected:
  vt::common::String test_file_;
};

DEFINE_TEST_F(CanMapFileToRead, MmapTest) {
  vt::common::FileDescriptor fd(test_file_.c_str(), O_RDONLY);
  {
    vt::common::Mmap<PROT_READ> uut(3, MAP_PRIVATE, fd.handle(), 0);
    EXPECT_TRUE(uut.valid());
    EXPECT_EQ(uut.base()[0], 'a');
    EXPECT_EQ(uut.base()[1], 'b');
    EXPECT_EQ(uut.base()[2], 'c');
  }
}

DEFINE_TEST_F(CanMapFileToWrite, MmapTest) {
  {
    vt::common::FileDescriptor fd(test_file_.c_str(), O_RDWR);
    vt::common::Mmap<PROT_WRITE> uut(3, MAP_SHARED, fd.handle(), 0);
    EXPECT_TRUE(uut.valid());
    uut.mutable_base()[1] = 'd';
  }

  vt::common::FileDescriptor fd(test_file_.c_str(), O_RDONLY);

  char buf[3];
  EXPECT_NE(read(fd.handle(), buf, 3), -1);
  EXPECT_EQ(buf[0], 'a');
  EXPECT_EQ(buf[1], 'd');
  EXPECT_EQ(buf[2], 'c');
}

DEFINE_TEST_F(CanBeMoved, MmapTest) {
  vt::common::FileDescriptor fd(test_file_.c_str(), O_RDONLY);
  vt::common::Mmap<PROT_READ> uut(3, MAP_PRIVATE, fd.handle(), 0);
  vt::common::Mmap<PROT_READ> uut2(vt::move(uut));
  EXPECT_FALSE(uut.valid());
  EXPECT_TRUE(uut2.valid());
}