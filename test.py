# tests
# coding=UTF-8

import unittest
import keyframe_snap_cut
import pprint
import shutil
import os

SOURCE_AVI = "H264_test7_voiceclip_avi_480x360.avi"

class TestThings(unittest.TestCase):
    weird_filename = 'Haus-M\xc3\xbctter.avi'

    def setUp(self):
        shutil.copyfile(SOURCE_AVI, self.weird_filename)

    def tearDown(self):
        os.remove(self.weird_filename)

    def test_weird_filenames(self):
        obj = keyframe_snap_cut.KeyframeSnapCut()
        result = obj.get_keyframes(self.weird_filename)
        pprint.pprint(result)
        self.assertEqual(2+2, 4)


if __name__ == "__main__":
    unittest.main()
