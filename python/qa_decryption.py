#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @file
# @author (C) 2015 by Roman Khassraf <rkhassraf@gmail.com>
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

from gnuradio import gr, gr_unittest, blocks
import grgsm_swig as grgsm
import pmt

class qa_decryption (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    def test_001_a51 (self):
        """
            A system information message on the SACCH of TCH/F, encrypted with A5/1
        """
        framenumbers_input = [1259192, 1259218, 1259244, 1259270]
        timeslots_input = [2, 2, 2, 2]
        bursts_input = [
            "0001100001000111100111101111100101000100101011000010011110011101001111101100010100111111100000110100011111101011101100100111110011000100010001010000",
            "0001000101000000001001111110000110010110110111110111101000001101001111101100010100111111001110001001110101110001010001000111011010010001011011000000",
            "0001001101101101000111001000101011001101001110110001001100111101001111101100010100111111111001001010011010011111010010010101011001001011011100110000",
            "0000010010100000001001101010100001011100010001101100111111101101001111101100010100111111101101001110100010101110010110101111100010010000110010110000"
        ]
        bursts_expected = [
            "0000010111000110010001010000000101010011110101100000100000011101001111101100010100111111010110110000000001101110000101000000000101000100011000110000",
            "0000010110101100010100111101011001000000000101010011000100001101001111101100010100111111011001101001110000100001000110000000101100010111100111010000",
            "0000011110110111011011100001010000000000110100100000100001001101001111101100010100111111100010000000000000001101000000100000010011001110100000010000",
            "0000011000010001000000001101000001001001000010001000000000001101001111101100010100111111000010110001001110000000110111001110010000010111000111001000"
        ]

        key = [0x32,0xE5,0x45,0x53,0x20,0x8C,0xE0,0x00]
        a5_version = 1

        src = grgsm.burst_source(framenumbers_input, timeslots_input, bursts_input)
        decryption = grgsm.decryption((key), a5_version)
        dst = grgsm.burst_sink()

        self.tb.msg_connect(src, "out", decryption, "bursts")
        self.tb.msg_connect(decryption, "bursts", dst, "in")

        self.tb.run ()

        # have to convert tuple to list
        framenumbers_result = list(dst.get_framenumbers())
        timeslots_result = list(dst.get_timeslots())
        bursts_result = list(dst.get_burst_data())
        
        self.assertEqual(framenumbers_input, framenumbers_result)
        self.assertEqual(timeslots_input, timeslots_result)
        self.assertEqual(bursts_expected, bursts_result)
        

    def test_002_a51 (self):
        """
            A TMSI Reallocation command on SDCCH/8, encrypted with A5/1
        """
        framenumbers_input = [2569043, 2569044, 2569045, 2569046]
        timeslots_input = [4, 4, 4, 4]
        bursts_input = [
            "0000111101111110011111000111000100110100001101100001000110011000110101110010000011010111010110101100100010011000000100111010001000011000010010010000",
            "0001010010001100110000000111100110101111001001101111000000101000110101110010000011010111001101001101000001000001110101101100101111010011001000111000",
            "0001110111101000110100001111000010100001101011000001010010011000110101110010000011010111101110000011100010110110101010100101010011011111111001000000",
            "0001111011000100011010100010000110001101111001000110010100001000110101110010000011010111000100101011110110000100110110001110010011110110110101100000"
        ]
        bursts_expected = [
            "0001100000010010011110111110011111000000001010001111000000001000110101110010000011010111100101101010000001111010100010110111101011101011100000101000",
            "0001000101111101111110000010100001011011111010111110101011101000110101110010000011010111110110111101101111110000011011010111011111001011101000011000",
            "0000001000011110111110101011001000110000000000110110101100011000110101110010000011010111001010100101011111001000111100000100000111111000000101110000",
            "0001101010111110010001010110101100000011101100011111110100101000110101110010000011010111111000000001010010111001111111011001000000001001000011101000"
        ]
                
        key = [0xAD,0x6A,0x3E,0xC2,0xB4,0x42,0xE4,0x00]
        a5_version = 1
        
        src = grgsm.burst_source(framenumbers_input, timeslots_input, bursts_input)
        decryption = grgsm.decryption((key), a5_version)
        dst = grgsm.burst_sink()

        self.tb.msg_connect(src, "out", decryption, "bursts")
        self.tb.msg_connect(decryption, "bursts", dst, "in")
        
        self.tb.run ()
                
        # have to convert tuple to list
        framenumbers_result = list(dst.get_framenumbers())
        timeslots_result = list(dst.get_timeslots())
        bursts_result = list(dst.get_burst_data())
        
        self.assertEqual(framenumbers_input, framenumbers_result)
        self.assertEqual(timeslots_input, timeslots_result)
        self.assertEqual(bursts_expected, bursts_result)


    def test_003_a53 (self):
        """
            A cp-ack message of a SMS, encrypted with A5/3
        """
        framenumbers_input = [1935011, 1935012, 1935013, 1935014]
        timeslots_input = [2, 2, 2, 2]
        bursts_input = [
            "0001111001001110001101111101111111110100011010101100100001011101001111101100010100111111101101011110100011101111001000110110100101101011110010100000",
            "0001111000110011010110000111010010100101001100111011000001011101001111101100010100111111000100101000001011010001100000010100011000011111001111011000",
            "0000000110100101110010011101101100101110001100000000101001011101001111101100010100111111100100100010110110111011010101010001001100010100100100111000",
            "0000011100111011101010000111001010010001100110011011100101011101001111101100010100111111101110110100101101010100111101000000111001011011100010101000"
        ]
        bursts_expected = [
            "0001000000000010100111000010010101001010011110010010101110011101001111101100010100111111011010111010111100110000011100111010001010100010100110000000",
            "0000101110001111011110100111101010000000101101101011101001011101001111101100010100111111010110100111001100100011000100100011110101001010110001001000",
            "0000001111011010110111000100111111000011001010100011000110011101001111101100010100111111100111100010011100000010110011100001101000000000000011001000",
            "0000011011100010001000101000101010010011010000100011110011001101001111101100010100111111010100100010010100111010101110001101101110101110011100101000"
        ]
                
        key = [0x41,0xBC,0x19,0x30,0xB6,0x31,0x8A,0xC8]
        a5_version = 3
        
        src = grgsm.burst_source(framenumbers_input, timeslots_input, bursts_input)
        decryption = grgsm.decryption((key), a5_version)
        dst = grgsm.burst_sink()

        self.tb.msg_connect(src, "out", decryption, "bursts")
        self.tb.msg_connect(decryption, "bursts", dst, "in")
        
        self.tb.run ()
                
        # have to convert tuple to list
        framenumbers_result = list(dst.get_framenumbers())
        timeslots_result = list(dst.get_timeslots())
        bursts_result = list(dst.get_burst_data())
        
        self.assertEqual(framenumbers_input, framenumbers_result)
        self.assertEqual(timeslots_input, timeslots_result)
        self.assertEqual(bursts_expected, bursts_result)


    def test_004_a53 (self):
        """
            An assignment command for a TCH channel, encrypted with A5/3
        """
        framenumbers_input = [435897, 435898, 435899, 435900]
        timeslots_input = [2, 2, 2, 2]
        bursts_input = [
            "0001001000010110001000001101001010100000011100011011110101011101001111101100010100111111010000100000100101101111000010001100001000100101100101010000",
            "0000011101010011010110101000011011101010100001011001100011001101001111101100010100111111000110011001110101110111000100101001111100110100011011011000",
            "0000000000110011000001110101110101111011011111000111101001011101001111101100010100111111101100010011010000010001101101000110000011011000011100011000",
            "0000000001110011001010110101100110100111110010000101001011111101001111101100010100111111101110001101111111001001001000101101010110010101010110100000",
        ]
        bursts_expected = [
            "0001101100011001110111101010110000001111000010110011000110101101001111101100010100111111100010000011100010101001010110101100001111101111110010011000",
            "0001101001110110000111000011111110011011001001101010011000001101001111101100010100111111110010001001001001101011111010010100100011100110110000011000",
            "0001000001110000001011101010011010010100010010100110010010001101001111101100010100111111010011011101010110100000111111011111100000010100000111000000",
            "0000001000001010010001010000101011101100100100001010011101111101001111101100010100111111000001001001100100101010000011101010100001110000100000001000"
        ]
                
        key = [0xAD,0x2C,0xB3,0x83,0x2F,0x4A,0x6C,0xF1]
        a5_version = 3
        
        src = grgsm.burst_source(framenumbers_input, timeslots_input, bursts_input)
        decryption = grgsm.decryption((key), a5_version)
        dst = grgsm.burst_sink()

        self.tb.msg_connect(src, "out", decryption, "bursts")
        self.tb.msg_connect(decryption, "bursts", dst, "in")
        
        self.tb.run ()
                
        # have to convert tuple to list
        framenumbers_result = list(dst.get_framenumbers())
        timeslots_result = list(dst.get_timeslots())
        bursts_result = list(dst.get_burst_data())
        
        self.assertEqual(framenumbers_input, framenumbers_result)
        self.assertEqual(timeslots_input, timeslots_result)
        self.assertEqual(bursts_expected, bursts_result)


if __name__ == '__main__':
    gr_unittest.run(qa_decryption, "qa_decryption.xml")
