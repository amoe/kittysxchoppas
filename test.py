#! /usr/bin/python3
# coding=UTF-8

# main test suite

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
        expected_result = [
            0.0,
            0.5,
            1.0,
            1.5,
            2.0,
            2.5,
            3.0,
            3.5,
            4.0,
            4.5,
            5.0,
            5.5,
            6.0,
            6.5,
            7.0,
            7.5,
            8.0,
            8.5,
            9.0,
            9.5,
            10.0,
            10.5
        ]            
      
        self.assertEqual(result, expected_result)


if __name__ == "__main__":
    unittest.main()
