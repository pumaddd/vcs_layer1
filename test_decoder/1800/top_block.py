#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Fri Nov 22 17:55:59 2019
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from grc_gnuradio import wxgui as grc_wxgui
from grgsm import arfcn
from math import pi
from optparse import OptionParser
import grgsm
import math
import time
import wx


class top_block(grc_wxgui.top_block_gui):

    def __init__(self, b=5e6, base_port="6700", f="/home/datqm/Desktop/Share_Folder/Share_Folder/test_613.cfile", fc=[1856.4e6,1856.4e6+5e6,1856.4e6 + 10e6,1856.4e6-10e6,1856.4-5e6], fn=0, hsn=21, id_start=1, ma=[726,806,808,811], maio=2, n="offsetCFile", nstream=5, osr=4, s=3.25e6, s1=25e6/9, sample_start=30099, tseq=2, tslot=1):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")

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
        self.s1 = s1
        self.sample_start = sample_start
        self.tseq = tseq
        self.tslot = tslot

        ##################################################
        # Variables
        ##################################################
        self.gsm_symb_rate = gsm_symb_rate = 1625000.0/6.0
        self.samp_rate_out = samp_rate_out = gsm_symb_rate*osr
        self.samp_rate = samp_rate = 20e6

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(2),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(1855.2e6, 0)
        self.uhd_usrp_source_0.set_gain(30, 0)
        self.uhd_usrp_source_0.set_bandwidth(samp_rate, 0)
        self.uhd_usrp_source_0.set_center_freq(1761.4e6, 1)
        self.uhd_usrp_source_0.set_gain(30, 1)
        self.uhd_usrp_source_0.set_bandwidth(samp_rate, 1)
        self.gsm_file_sink_6 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/media/datqm/sdb2/downlink/file_01/", "1855200000", samp_rate/5, "Down", False)
        self.gsm_file_sink_5 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/media/datqm/sdb2/downlink/file_03/", "1863200000", samp_rate/5, "Down", False)
        self.gsm_file_sink_4 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/media/datqm/sdb2/downlink/file_04/", "1847200000", samp_rate/5, "Down", False)
        self.gsm_file_sink_3 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/data/uplink/file_01/", "1855200000", samp_rate/5, "Up", False)
        self.gsm_file_sink_2 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/media/datqm/sdb2/downlink/file_02/", "1859200000", samp_rate/5, "Down", False)
        self.gsm_file_sink_1_0_0_0 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/data/uplink/file_05/", "1851200000", samp_rate/5, "Up", False)
        self.gsm_file_sink_1_0_0 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/data/uplink/file_04/", "1847200000", samp_rate/5, "Up", False)
        self.gsm_file_sink_1_0 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/data/uplink/file_03/", "1863200000", samp_rate/5, "Up", False)
        self.gsm_file_sink_1 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/data/uplink/file_02/", "1859200000", samp_rate/5, "Up", False)
        self.gsm_file_sink_0 = grgsm.file_sink(gr.sizeof_gr_complex*1, "/media/datqm/sdb2/downlink/file_05/", "1851200000", samp_rate/5, "Down", False)
        self.gsm_decimator_ccf_2 = grgsm.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, samp_rate, samp_rate/10, 5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.gsm_decimator_ccf_2.declare_sample_delay(0)
        	
        self.gsm_decimator_ccf_1 = grgsm.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, samp_rate, samp_rate/10, 5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.gsm_decimator_ccf_1.declare_sample_delay(0)
        	

        ##################################################
        # Connections
        ##################################################
        self.connect((self.gsm_decimator_ccf_1, 1), (self.gsm_file_sink_1, 0))    
        self.connect((self.gsm_decimator_ccf_1, 2), (self.gsm_file_sink_1_0, 0))    
        self.connect((self.gsm_decimator_ccf_1, 3), (self.gsm_file_sink_1_0_0, 0))    
        self.connect((self.gsm_decimator_ccf_1, 4), (self.gsm_file_sink_1_0_0_0, 0))    
        self.connect((self.gsm_decimator_ccf_1, 0), (self.gsm_file_sink_3, 0))    
        self.connect((self.gsm_decimator_ccf_2, 4), (self.gsm_file_sink_0, 0))    
        self.connect((self.gsm_decimator_ccf_2, 1), (self.gsm_file_sink_2, 0))    
        self.connect((self.gsm_decimator_ccf_2, 3), (self.gsm_file_sink_4, 0))    
        self.connect((self.gsm_decimator_ccf_2, 2), (self.gsm_file_sink_5, 0))    
        self.connect((self.gsm_decimator_ccf_2, 0), (self.gsm_file_sink_6, 0))    
        self.connect((self.uhd_usrp_source_0, 1), (self.gsm_decimator_ccf_1, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.gsm_decimator_ccf_2, 0))    

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

    def get_s1(self):
        return self.s1

    def set_s1(self, s1):
        self.s1 = s1

    def get_sample_start(self):
        return self.sample_start

    def set_sample_start(self, sample_start):
        self.sample_start = sample_start

    def get_tseq(self):
        return self.tseq

    def set_tseq(self, tseq):
        self.tseq = tseq

    def get_tslot(self):
        return self.tslot

    def set_tslot(self, tslot):
        self.tslot = tslot

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
        self.gsm_decimator_ccf_1.set_taps((firdes.low_pass(1, self.samp_rate, self.samp_rate/10, 5e3,firdes.WIN_HAMMING,6.76)))
        self.gsm_decimator_ccf_2.set_taps((firdes.low_pass(1, self.samp_rate, self.samp_rate/10, 5e3,firdes.WIN_HAMMING,6.76)))
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.uhd_usrp_source_0.set_bandwidth(self.samp_rate, 0)
        self.uhd_usrp_source_0.set_bandwidth(self.samp_rate, 1)


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--b", dest="b", type="eng_float", default=eng_notation.num_to_str(5e6),
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
        "", "--maio", dest="maio", type="intx", default=2,
        help="Set maio [default=%default]")
    parser.add_option(
        "", "--n", dest="n", type="string", default="offsetCFile",
        help="Set Database name [default=%default]")
    parser.add_option(
        "", "--nstream", dest="nstream", type="intx", default=5,
        help="Set Number Stream [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=4,
        help="Set OSR [default=%default]")
    parser.add_option(
        "", "--s", dest="s", type="eng_float", default=eng_notation.num_to_str(3.25e6),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--s1", dest="s1", type="eng_float", default=eng_notation.num_to_str(25e6/9),
        help="Set samp_rate_in [default=%default]")
    parser.add_option(
        "", "--sample-start", dest="sample_start", type="intx", default=30099,
        help="Set Sample Start [default=%default]")
    parser.add_option(
        "", "--tseq", dest="tseq", type="intx", default=2,
        help="Set Traning Sequence [default=%default]")
    parser.add_option(
        "", "--tslot", dest="tslot", type="intx", default=1,
        help="Set Time Slot [default=%default]")
    return parser


def main(top_block_cls=top_block, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print "Error: failed to enable real-time scheduling."

    tb = top_block_cls(b=options.b, base_port=options.base_port, f=options.f, fn=options.fn, hsn=options.hsn, id_start=options.id_start, maio=options.maio, n=options.n, nstream=options.nstream, osr=options.osr, s=options.s, s1=options.s1, sample_start=options.sample_start, tseq=options.tseq, tslot=options.tslot)
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
