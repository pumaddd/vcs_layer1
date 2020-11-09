#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Thu Jul 11 03:17:12 2019
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from grgsm import arfcn
from math import pi
from optparse import OptionParser
import grgsm
import math
import pmt


class top_block(gr.top_block):

    def __init__(self, fc0=1825.4e6, s=45.5e6, fc=1835e6, osr=4, f="/home/datqm/Desktop/Share_Folder/Share_Folder/test_613.cfile", n="offsetCFile"):
        gr.top_block.__init__(self, "Top Block")

        ##################################################
        # Parameters
        ##################################################
        self.fc0 = fc0
        self.s = s
        self.fc = fc
        self.osr = osr
        self.f = f
        self.n = n

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_out = samp_rate_out = 1625000*4/6

        ##################################################
        # Blocks
        ##################################################
        self.pfb_decimator_ccf_0 = pfb.decimator_ccf(
        	  int(s/samp_rate_out),
        	  (firdes.low_pass(1, s,125e3,5.0e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0.declare_sample_delay(0)
        	
        self.gsm_message_printer_0 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_decodeC0_0 = grgsm.decodeC0(osr, ([arfcn.downlink2arfcn(fc0)]), fc, fc0, s, n, False)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.gsm_bcch_ccch_demapper_0 = grgsm.gsm_bcch_ccch_demapper(
            timeslot_nr=0,
        )
        self.blocks_rotator_cc_0 = blocks.rotator_cc(-2*pi*(fc0 - fc)/s)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, f, False)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_bcch_ccch_demapper_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_0, 'msgs'))    
        self.msg_connect((self.gsm_decodeC0_0, 'C0'), (self.gsm_bcch_ccch_demapper_0, 'bursts'))    
        self.connect((self.blocks_file_source_0, 0), (self.blocks_rotator_cc_0, 0))    
        self.connect((self.blocks_rotator_cc_0, 0), (self.pfb_decimator_ccf_0, 0))    
        self.connect((self.gsm_decodeC0_0, 0), (self.blocks_null_sink_0, 0))    
        self.connect((self.pfb_decimator_ccf_0, 0), (self.gsm_decodeC0_0, 0))    

    def get_fc0(self):
        return self.fc0

    def set_fc0(self, fc0):
        self.fc0 = fc0
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)

    def get_s(self):
        return self.s

    def set_s(self, s):
        self.s = s
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)
        self.pfb_decimator_ccf_0.set_taps((firdes.low_pass(1, self.s,125e3,5.0e3,firdes.WIN_HAMMING,6.76)))

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr

    def get_f(self):
        return self.f

    def set_f(self, f):
        self.f = f
        self.blocks_file_source_0.open(self.f, False)

    def get_n(self):
        return self.n

    def set_n(self, n):
        self.n = n

    def get_samp_rate_out(self):
        return self.samp_rate_out

    def set_samp_rate_out(self, samp_rate_out):
        self.samp_rate_out = samp_rate_out


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--fc0", dest="fc0", type="eng_float", default=eng_notation.num_to_str(1825.4e6),
        help="Set frequency CH0 [default=%default]")
    parser.add_option(
        "", "--s", dest="s", type="eng_float", default=eng_notation.num_to_str(45.5e6),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--fc", dest="fc", type="eng_float", default=eng_notation.num_to_str(1835e6),
        help="Set frequency center [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=4,
        help="Set OSR [default=%default]")
    parser.add_option(
        "", "--f", dest="f", type="string", default="/home/datqm/Desktop/Share_Folder/Share_Folder/test_613.cfile",
        help="Set CH0 file path [default=%default]")
    parser.add_option(
        "", "--n", dest="n", type="string", default="offsetCFile",
        help="Set Name of database [default=%default]")
    return parser


def main(top_block_cls=top_block, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(fc0=options.fc0, s=options.s, fc=options.fc, osr=options.osr, f=options.f, n=options.n)
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
