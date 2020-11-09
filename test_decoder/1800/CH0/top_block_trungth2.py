#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block Trungth2
# Generated: Wed May 20 17:53:19 2020
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


class top_block_trungth2(gr.top_block):

    def __init__(self, fc=945.5e6, fc0=arfcn.arfcn2downlink(53), fc_0=947.5e6, n="53_12717_11413", osr=2, s=2e6):
        gr.top_block.__init__(self, "Top Block Trungth2")

        ##################################################
        # Parameters
        ##################################################
        self.fc = fc
        self.fc0 = fc0
        self.fc_0 = fc_0
        self.n = n
        self.osr = osr
        self.s = s

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_out = samp_rate_out = 1625000*4/6

        ##################################################
        # Blocks
        ##################################################
        self.pfb_decimator_ccf_0 = pfb.decimator_ccf(
        	  4,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.5),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)
        	
        self.gsm_message_printer_0 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_file_source_C0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, "945500000", False)
        self.gsm_decodeC0_0 = grgsm.decodeC0(osr, ([arfcn.downlink2arfcn(fc0)]), fc_0, fc0, s, 0, n, False)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.gsm_bcch_ccch_demapper_0 = grgsm.gsm_bcch_ccch_demapper(
            timeslot_nr=0,
        )
        self.blocks_rotator_cc_0 = blocks.rotator_cc(-2*pi*(fc0 - fc)/s)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_bcch_ccch_demapper_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_0, 'msgs'))    
        self.msg_connect((self.gsm_decodeC0_0, 'C0'), (self.gsm_bcch_ccch_demapper_0, 'bursts'))    
        self.connect((self.blocks_rotator_cc_0, 0), (self.pfb_decimator_ccf_0, 0))    
        self.connect((self.gsm_file_source_C0_0, 0), (self.blocks_rotator_cc_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.gsm_decodeC0_0, 0))    
        self.connect((self.pfb_decimator_ccf_0, 0), (self.pfb_arb_resampler_xxx_0, 0))    

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)

    def get_fc0(self):
        return self.fc0

    def set_fc0(self, fc0):
        self.fc0 = fc0
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)

    def get_fc_0(self):
        return self.fc_0

    def set_fc_0(self, fc_0):
        self.fc_0 = fc_0

    def get_n(self):
        return self.n

    def set_n(self, n):
        self.n = n

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr

    def get_s(self):
        return self.s

    def set_s(self, s):
        self.s = s
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)
        self.blocks_throttle_0.set_sample_rate(self.s)
        self.pfb_decimator_ccf_0.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))

    def get_samp_rate_out(self):
        return self.samp_rate_out

    def set_samp_rate_out(self, samp_rate_out):
        self.samp_rate_out = samp_rate_out


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--fc", dest="fc", type="eng_float", default=eng_notation.num_to_str(945.5e6),
        help="Set frequency center [default=%default]")
    parser.add_option(
        "", "--fc0", dest="fc0", type="eng_float", default=eng_notation.num_to_str(arfcn.arfcn2downlink(53)),
        help="Set frequency CH0 [default=%default]")
    parser.add_option(
        "", "--fc-0", dest="fc_0", type="eng_float", default=eng_notation.num_to_str(947.5e6),
        help="Set frequency center capture [default=%default]")
    parser.add_option(
        "", "--n", dest="n", type="string", default="53_12717_11413",
        help="Set Name of database [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=2,
        help="Set OSR [default=%default]")
    parser.add_option(
        "", "--s", dest="s", type="eng_float", default=eng_notation.num_to_str(2e6),
        help="Set samp_rate [default=%default]")
    return parser


def main(top_block_cls=top_block_trungth2, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(fc=options.fc, fc0=options.fc0, fc_0=options.fc_0, n=options.n, osr=options.osr, s=options.s)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
