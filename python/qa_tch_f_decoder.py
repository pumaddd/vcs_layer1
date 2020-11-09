#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @file
# @author (C) 2018 by Vasil Velichkov <vvvelichkov@gmail.com>
# @section LICENSE
# 
# Gr-gsm is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# Gr-gsm is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gr-gsm; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 
# 

import numpy as np
from gnuradio import gr, gr_unittest, blocks
import grgsm_swig as grgsm
import pmt

class qa_tch_f_decoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_fr (self):
        """
            Simple TCH/F Full Rate test
        """
        framenumbers_input = [373420, 373421, 373422, 373423, 373424, 373425, 373426, 373427, 3734298]
        timeslots_input = [3, 3, 3, 3, 3, 3, 3, 3]
        bursts_input = [
                "0000101011011010011110011011101011100000100010001011010100110111011110001001011101111000111010001001000110000011100010000010111110110101001101011000",
                "0001011011111010000000111010110001110101010011011100000110100111011110001001011101111000110100000010000001100010101101011010000110000101101011000000",
                "0000011010010000100000000100110111111110101101011001001010010111011110001001011101111000010001010001101000010100100111101110110010011110010000011000",
                "0001000000010111010111100100110110000101101111000000011011100111011110001001011101111000101001000001101010010110011100011010010011010001010010001000",
                "0000111001001110100011110000101001110110111011001110110110110111011110001001011101111000011110001000010101011000101010010101000000100000010011101000",
                "0000011001001010010101100000110100101001111001001100101010000111011110001001011101111000100111000000111111000000100001101001101010000110111111100000",
                "0001011011101100100001010001001000110010010010100101000111010111011110001001011101111000100001101110100101000100100100100100000100011101111001011000",
                "0000010111000101101101100001011000110010010011001110101000010111011110001001011101111000001001100011010001111110001101010100110001010001100100001000"
        ]

        src = grgsm.burst_source(framenumbers_input, timeslots_input, bursts_input)
        decoder = grgsm.tch_f_decoder(grgsm.TCH_FS, False)
        dst = blocks.message_debug()

        self.tb.msg_connect(src, "out", decoder, "bursts")
        self.tb.msg_connect(decoder, "voice", dst, "store")

        self.tb.run ()

        self.assertEqual(dst.num_messages(), 1)

        pdu = dst.get_message(0)
        self.assertEqual(True, pmt.is_pair(pdu))

        data = pmt.cdr(pdu)
        self.assertEqual(True, pmt.is_blob(data))

        np.testing.assert_array_equal(
                np.array([
                    211,  92, 197, 118, 171, 142, 160,  70, 219, 146,  71,  20, 226,
                    128,  73,  35, 142,  75,  35,  94,  32,  73,  28, 114,  73,  44,
                    132, 192,  72, 228, 141, 201,  27
                    ]),
                pmt.to_python(data))

    def test_fr_parity_error (self):
        """
            Test parity error
        """
        framenumbers_input = [371462, 371463, 371464, 371465, 371466, 371467, 371468, 371469]
        timeslots_input = [3, 3, 3, 3, 3, 3, 3, 3]
        bursts_input = [
                "0001000010000011000010100000100101100000101101011000100000010111011110001001011101111001011010000000111010000000101101100000001010000000000000110000",
                "0001010101010101010010001011010101010101111101001000000001100111011110001001011101111001010101011101010000000010000001011101110001001010001001000000",
                "0000100010110100100000001001001010000000101110100000001111000111011110001001011101111001000000001101000000001010010000000010100100101000000000011000",
                "0000000010010101111111010011001000000000101101010111010100000111011110001001011101111001000010000001010101110001000000000010010001110111110000000000",
                "0001000000100011101010100000000101000000001111111010100000011111011110001001011101111001010000100010111010100000000101000010000010101010100000010000",
                "0001010101111111111010000001010101011111111111101000000001001111011110001001011101111001010111111111111010101000000001010101011011101010000001000000",
                "0000000000111110101010100001000000100010101110101010000101001111011110001001011101111001001010001111101010001000010000000000101110101010100000010000",
                "0000001000010101010101111011101010100000000101011101111110101111011110001001011101111001100010000001010101011011101010001000010001011101111010101000"
        ]

        src = grgsm.burst_source(framenumbers_input, timeslots_input, bursts_input)
        decoder = grgsm.tch_f_decoder(grgsm.TCH_FS, False)
        dst = blocks.message_debug()

        self.tb.msg_connect(src, "out", decoder, "bursts")
        self.tb.msg_connect(decoder, "voice", dst, "store")

        self.tb.run ()

        self.assertEqual(dst.num_messages(), 0)

if __name__ == '__main__':
    gr_unittest.run(qa_tch_f_decoder, "qa_tch_f_decoder.xml")
