#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Decode All
# Generated: Sat Nov 16 18:03:02 2019
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from math import pi
from optparse import OptionParser
import grgsm
import math
import pmt


class decode_all(gr.top_block):

    def __init__(self, b=4e6, base_port="6700", f="/home/datqm/Desktop/Share_Folder/Share_Folder/test_613.cfile", fc=[1848.4e6, 18644.4e6], fn=0, hsn=21, id_start=1, ma=[726,806,808,811], maio=0, n="726", nstream=2, osr=2, s=4e6, tseq=2, tslot=1, sample_start=0):
        gr.top_block.__init__(self, "Decode All")

        ##################################################
        # Parameters
        ##################################################
        self.b = b
        self.base_port = base_port
        self.f = f
        self.fc = fc
        self.fn = fn
        self.hsn = hsn
        self.id_start = id_start
        self.ma = ma
        self.maio = maio
        self.n = n
        self.nstream = nstream
        self.osr = osr
        self.s = s
        self.tseq = tseq
        self.tslot = tslot
        self.sample_start = sample_start

        ##################################################
        # Variables
        ##################################################
        self.gsm_symb_rate = gsm_symb_rate = 1625000.0/6.0
        self.samp_rate_out = samp_rate_out = gsm_symb_rate*osr
        self.samp_rate = samp_rate = 32000
        self.ppm = ppm = -0.661913

        ##################################################
        # Blocks
        ##################################################
        self.pfb_decimator_ccf_0_5 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_5.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0_4 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_4.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0_3 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_3.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0_2 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_2.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0_1 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_1.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0_0_1 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_0_1.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0_0 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_0.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_3_2 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_3_2.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_3_1 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_3_1.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_3_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_3_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_3 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_3.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_2 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_2.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_1 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_1.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)
        	
        self.gsm_sdcch8_demapper_0 = grgsm.gsm_sdcch8_demapper(
            timeslot_nr=1,
        )
        self.gsm_rotator_cx_0_0_0_0 = grgsm.rotator_cx(([811]), ([1864.4e6]), s, s, "726", id_start, s)
        self.gsm_rotator_cx_0_0_0 = grgsm.rotator_cx(([808]), ([1864.4e6]), s, s, "726", id_start, s)
        self.gsm_rotator_cx_0_0 = grgsm.rotator_cx(([806]), ([1864.4e6]), s, s, "726", id_start, s)
        self.gsm_rotator_cx_0 = grgsm.rotator_cx(([726]), ([1848.4e6]), s, s, "726", id_start, s)
        self.gsm_message_printer_0 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_file_source_C0_0_5 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1864400000", False)
        self.gsm_file_source_C0_0_4 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1848400000", True)
        self.gsm_file_source_C0_0_3 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1864400000", True)
        self.gsm_file_source_C0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1848400000", False)
        self.gsm_decode_cx_0 = grgsm.decode_cx(osr, ([726,806,808,811]), tseq, s, n, id_start)
        (self.gsm_decode_cx_0).set_processor_affinity((0, 1, 2, 3, 4, 5))
        self.gsm_cx_channel_hopper_0 = grgsm.cx_channel_hopper(([726,806]), 1, 21)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.blocks_throttle_0_2_2 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_throttle_0_2_1 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_throttle_0_2_0 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_throttle_0_2 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_0, 'msgs'))    
        self.msg_connect((self.gsm_cx_channel_hopper_0, 'bursts'), (self.gsm_sdcch8_demapper_0, 'bursts'))    
        self.msg_connect((self.gsm_decode_cx_0, 'CX'), (self.gsm_cx_channel_hopper_0, 'CX'))    
        self.msg_connect((self.gsm_sdcch8_demapper_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.connect((self.blocks_throttle_0_2, 0), (self.gsm_rotator_cx_0, 0))    
        self.connect((self.blocks_throttle_0_2_0, 0), (self.gsm_rotator_cx_0_0, 0))    
        self.connect((self.blocks_throttle_0_2_0, 0), (self.gsm_rotator_cx_0_0_0, 0))    
        self.connect((self.blocks_throttle_0_2_0, 0), (self.gsm_rotator_cx_0_0_0_0, 0))    
        self.connect((self.blocks_throttle_0_2_1, 0), (self.gsm_rotator_cx_0, 1))    
        self.connect((self.blocks_throttle_0_2_2, 0), (self.gsm_rotator_cx_0_0, 1))    
        self.connect((self.blocks_throttle_0_2_2, 0), (self.gsm_rotator_cx_0_0_0, 1))    
        self.connect((self.blocks_throttle_0_2_2, 0), (self.gsm_rotator_cx_0_0_0_0, 1))    
        self.connect((self.gsm_file_source_C0_0, 0), (self.blocks_throttle_0_2, 0))    
        self.connect((self.gsm_file_source_C0_0_3, 0), (self.blocks_throttle_0_2_2, 0))    
        self.connect((self.gsm_file_source_C0_0_4, 0), (self.blocks_throttle_0_2_1, 0))    
        self.connect((self.gsm_file_source_C0_0_5, 0), (self.blocks_throttle_0_2_0, 0))    
        self.connect((self.gsm_rotator_cx_0, 0), (self.pfb_decimator_ccf_0, 0))    
        self.connect((self.gsm_rotator_cx_0, 1), (self.pfb_decimator_ccf_0_5, 0))    
        self.connect((self.gsm_rotator_cx_0_0, 0), (self.pfb_decimator_ccf_0_3, 0))    
        self.connect((self.gsm_rotator_cx_0_0, 1), (self.pfb_decimator_ccf_0_4, 0))    
        self.connect((self.gsm_rotator_cx_0_0_0, 1), (self.pfb_decimator_ccf_0_1, 0))    
        self.connect((self.gsm_rotator_cx_0_0_0, 0), (self.pfb_decimator_ccf_0_2, 0))    
        self.connect((self.gsm_rotator_cx_0_0_0_0, 0), (self.pfb_decimator_ccf_0_0, 0))    
        self.connect((self.gsm_rotator_cx_0_0_0_0, 1), (self.pfb_decimator_ccf_0_0_1, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.gsm_decode_cx_0, 4))    
        self.connect((self.pfb_arb_resampler_xxx_0_0, 0), (self.gsm_decode_cx_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0_1, 0), (self.gsm_decode_cx_0, 1))    
        self.connect((self.pfb_arb_resampler_xxx_0_2, 0), (self.gsm_decode_cx_0, 5))    
        self.connect((self.pfb_arb_resampler_xxx_0_3, 0), (self.gsm_decode_cx_0, 2))    
        self.connect((self.pfb_arb_resampler_xxx_0_3_0, 0), (self.gsm_decode_cx_0, 6))    
        self.connect((self.pfb_arb_resampler_xxx_0_3_1, 0), (self.gsm_decode_cx_0, 7))    
        self.connect((self.pfb_arb_resampler_xxx_0_3_2, 0), (self.gsm_decode_cx_0, 3))    
        self.connect((self.pfb_decimator_ccf_0, 0), (self.pfb_arb_resampler_xxx_0_0, 0))    
        self.connect((self.pfb_decimator_ccf_0_0, 0), (self.pfb_arb_resampler_xxx_0_3_2, 0))    
        self.connect((self.pfb_decimator_ccf_0_0_1, 0), (self.pfb_arb_resampler_xxx_0_3_1, 0))    
        self.connect((self.pfb_decimator_ccf_0_1, 0), (self.pfb_arb_resampler_xxx_0_3_0, 0))    
        self.connect((self.pfb_decimator_ccf_0_2, 0), (self.pfb_arb_resampler_xxx_0_3, 0))    
        self.connect((self.pfb_decimator_ccf_0_3, 0), (self.pfb_arb_resampler_xxx_0_1, 0))    
        self.connect((self.pfb_decimator_ccf_0_4, 0), (self.pfb_arb_resampler_xxx_0_2, 0))    
        self.connect((self.pfb_decimator_ccf_0_5, 0), (self.pfb_arb_resampler_xxx_0, 0))    

    def get_b(self):
        return self.b

    def set_b(self, b):
        self.b = b

    def get_base_port(self):
        return self.base_port

    def set_base_port(self, base_port):
        self.base_port = base_port

    def get_f(self):
        return self.f

    def set_f(self, f):
        self.f = f

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc

    def get_fn(self):
        return self.fn

    def set_fn(self, fn):
        self.fn = fn

    def get_hsn(self):
        return self.hsn

    def set_hsn(self, hsn):
        self.hsn = hsn

    def get_id_start(self):
        return self.id_start

    def set_id_start(self, id_start):
        self.id_start = id_start

    def get_ma(self):
        return self.ma

    def set_ma(self, ma):
        self.ma = ma

    def get_maio(self):
        return self.maio

    def set_maio(self, maio):
        self.maio = maio

    def get_n(self):
        return self.n

    def set_n(self, n):
        self.n = n

    def get_nstream(self):
        return self.nstream

    def set_nstream(self, nstream):
        self.nstream = nstream

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr
        self.set_samp_rate_out(self.gsm_symb_rate*self.osr)

    def get_s(self):
        return self.s

    def set_s(self, s):
        self.s = s
        self.blocks_throttle_0_2.set_sample_rate(self.s)
        self.blocks_throttle_0_2_0.set_sample_rate(self.s)
        self.blocks_throttle_0_2_1.set_sample_rate(self.s)
        self.blocks_throttle_0_2_2.set_sample_rate(self.s)
        self.pfb_decimator_ccf_0.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_0.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_0_1.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_1.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_2.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_3.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_4.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))
        self.pfb_decimator_ccf_0_5.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))

    def get_tseq(self):
        return self.tseq

    def set_tseq(self, tseq):
        self.tseq = tseq

    def get_tslot(self):
        return self.tslot

    def set_tslot(self, tslot):
        self.tslot = tslot

    def get_sample_start(self):
        return self.sample_start

    def set_sample_start(self, sample_start):
        self.sample_start = sample_start

    def get_gsm_symb_rate(self):
        return self.gsm_symb_rate

    def set_gsm_symb_rate(self, gsm_symb_rate):
        self.gsm_symb_rate = gsm_symb_rate
        self.set_samp_rate_out(self.gsm_symb_rate*self.osr)

    def get_samp_rate_out(self):
        return self.samp_rate_out

    def set_samp_rate_out(self, samp_rate_out):
        self.samp_rate_out = samp_rate_out

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--b", dest="b", type="eng_float", default=eng_notation.num_to_str(4e6),
        help="Set Band width [default=%default]")
    parser.add_option(
        "", "--base-port", dest="base_port", type="string", default="6700",
        help="Set 6700 [default=%default]")
    parser.add_option(
        "", "--f", dest="f", type="string", default="/home/datqm/Desktop/Share_Folder/Share_Folder/test_613.cfile",
        help="Set File Path [default=%default]")
    parser.add_option(
        "", "--fn", dest="fn", type="intx", default=0,
        help="Set Frame Number [default=%default]")
    parser.add_option(
        "", "--hsn", dest="hsn", type="intx", default=21,
        help="Set hsn [default=%default]")
    parser.add_option(
        "", "--id-start", dest="id_start", type="intx", default=1,
        help="Set ID start [default=%default]")
    parser.add_option(
        "", "--maio", dest="maio", type="intx", default=0,
        help="Set maio [default=%default]")
    parser.add_option(
        "", "--n", dest="n", type="string", default="726",
        help="Set Database name [default=%default]")
    parser.add_option(
        "", "--nstream", dest="nstream", type="intx", default=2,
        help="Set Number Stream [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=2,
        help="Set OSR [default=%default]")
    parser.add_option(
        "", "--s", dest="s", type="eng_float", default=eng_notation.num_to_str(4e6),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--tseq", dest="tseq", type="intx", default=2,
        help="Set Traning Sequence [default=%default]")
    parser.add_option(
        "", "--tslot", dest="tslot", type="intx", default=1,
        help="Set Time Slot [default=%default]")
    parser.add_option(
        "", "--sample-start", dest="sample_start", type="intx", default=0,
        help="Set Sample Start [default=%default]")
    return parser


def main(top_block_cls=decode_all, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(b=options.b, base_port=options.base_port, f=options.f, fn=options.fn, hsn=options.hsn, id_start=options.id_start, maio=options.maio, n=options.n, nstream=options.nstream, osr=options.osr, s=options.s, tseq=options.tseq, tslot=options.tslot, sample_start=options.sample_start)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
