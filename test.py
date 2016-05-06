# tests

import unittest

class TestThings(unittest.TestCase):
    def test_blah(self):
        self.assertEqual(2+2, 4)


if __name__ == "__main__":
    unittest.main()
