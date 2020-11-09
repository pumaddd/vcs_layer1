#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Gr-gsm Livemon
# Author: Piotr Krysik
# Description: Interactive monitor of a single C0 channel with analysis performed by Wireshark (command to run wireshark: sudo wireshark -k -f udp -Y gsmtap -i lo)
# Generated: Thu Sep 19 14:36:56 2019
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

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from math import pi
from optparse import OptionParser
import osmosdr
import sip
import sys


class grgsm_livemon(gr.top_block, Qt.QWidget):

    def __init__(self, args="", collector="localhost", collectorport="4729", fc=941.8e6, gain=30, osr=4, ppm=0, samp_rate=2000000.052982, serverport="4729", shiftoff=400e3):
        gr.top_block.__init__(self, "Gr-gsm Livemon")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Gr-gsm Livemon")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "grgsm_livemon")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.args = args
        self.collector = collector
        self.collectorport = collectorport
        self.fc = fc
        self.gain = gain
        self.osr = osr
        self.ppm = ppm
        self.samp_rate = samp_rate
        self.serverport = serverport
        self.shiftoff = shiftoff

        ##################################################
        # Variables
        ##################################################
        self.ppm_slider = ppm_slider = ppm
        self.gain_slider = gain_slider = gain
        self.fc_slider = fc_slider = fc

        ##################################################
        # Blocks
        ##################################################
        self._ppm_slider_range = Range(-150, 150, 0.1, ppm, 100)
        self._ppm_slider_win = RangeWidget(self._ppm_slider_range, self.set_ppm_slider, "PPM Offset", "counter", float)
        self.top_layout.addWidget(self._ppm_slider_win)
        self._gain_slider_range = Range(0, 100, 0.5, gain, 100)
        self._gain_slider_win = RangeWidget(self._gain_slider_range, self.set_gain_slider, "Gain", "counter", float)
        self.top_layout.addWidget(self._gain_slider_win)
        self._fc_slider_range = Range(800e6, 1990e6, 2e5, fc, 100)
        self._fc_slider_win = RangeWidget(self._fc_slider_range, self.set_fc_slider, "Frequency", "counter_slider", float)
        self.top_layout.addWidget(self._fc_slider_win)
        self.rtlsdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + args )
        self.rtlsdr_source_0.set_sample_rate(samp_rate)
        self.rtlsdr_source_0.set_center_freq(fc_slider-shiftoff, 0)
        self.rtlsdr_source_0.set_freq_corr(ppm_slider, 0)
        self.rtlsdr_source_0.set_dc_offset_mode(2, 0)
        self.rtlsdr_source_0.set_iq_balance_mode(2, 0)
        self.rtlsdr_source_0.set_gain_mode(False, 0)
        self.rtlsdr_source_0.set_gain(gain_slider, 0)
        self.rtlsdr_source_0.set_if_gain(20, 0)
        self.rtlsdr_source_0.set_bb_gain(20, 0)
        self.rtlsdr_source_0.set_antenna("", 0)
        self.rtlsdr_source_0.set_bandwidth(250e3+abs(shiftoff), 0)
          
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	fc_slider, #fc
        	samp_rate, #bw
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)
        
        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()
        
        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
        self.blocks_socket_pdu_0_1 = blocks.socket_pdu("UDP_CLIENT", collector, collectorport, 1500, False)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("UDP_SERVER", "127.0.0.1", serverport, 10000, False)
        self.blocks_rotator_cc_0 = blocks.rotator_cc(-2*pi*shiftoff/samp_rate)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_rotator_cc_0, 0), (self.blocks_null_sink_0, 0))    
        self.connect((self.blocks_rotator_cc_0, 0), (self.qtgui_freq_sink_x_0, 0))    
        self.connect((self.rtlsdr_source_0, 0), (self.blocks_rotator_cc_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "grgsm_livemon")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_args(self):
        return self.args

    def set_args(self, args):
        self.args = args

    def get_collector(self):
        return self.collector

    def set_collector(self, collector):
        self.collector = collector

    def get_collectorport(self):
        return self.collectorport

    def set_collectorport(self, collectorport):
        self.collectorport = collectorport

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.set_fc_slider(self.fc)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.set_gain_slider(self.gain)

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm
        self.set_ppm_slider(self.ppm)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*self.shiftoff/self.samp_rate)
        self.qtgui_freq_sink_x_0.set_frequency_range(self.fc_slider, self.samp_rate)
        self.rtlsdr_source_0.set_sample_rate(self.samp_rate)

    def get_serverport(self):
        return self.serverport

    def set_serverport(self, serverport):
        self.serverport = serverport

    def get_shiftoff(self):
        return self.shiftoff

    def set_shiftoff(self, shiftoff):
        self.shiftoff = shiftoff
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*self.shiftoff/self.samp_rate)
        self.rtlsdr_source_0.set_center_freq(self.fc_slider-self.shiftoff, 0)
        self.rtlsdr_source_0.set_bandwidth(250e3+abs(self.shiftoff), 0)

    def get_ppm_slider(self):
        return self.ppm_slider

    def set_ppm_slider(self, ppm_slider):
        self.ppm_slider = ppm_slider
        self.rtlsdr_source_0.set_freq_corr(self.ppm_slider, 0)

    def get_gain_slider(self):
        return self.gain_slider

    def set_gain_slider(self, gain_slider):
        self.gain_slider = gain_slider
        self.rtlsdr_source_0.set_gain(self.gain_slider, 0)

    def get_fc_slider(self):
        return self.fc_slider

    def set_fc_slider(self, fc_slider):
        self.fc_slider = fc_slider
        self.qtgui_freq_sink_x_0.set_frequency_range(self.fc_slider, self.samp_rate)
        self.rtlsdr_source_0.set_center_freq(self.fc_slider-self.shiftoff, 0)


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--args", dest="args", type="string", default="",
        help="Set Device Arguments [default=%default]")
    parser.add_option(
        "", "--collector", dest="collector", type="string", default="localhost",
        help="Set IP or DNS name of collector point [default=%default]")
    parser.add_option(
        "", "--collectorport", dest="collectorport", type="string", default="4729",
        help="Set UDP port number of collector [default=%default]")
    parser.add_option(
        "-f", "--fc", dest="fc", type="eng_float", default=eng_notation.num_to_str(941.8e6),
        help="Set GSM channel's central frequency [default=%default]")
    parser.add_option(
        "-g", "--gain", dest="gain", type="eng_float", default=eng_notation.num_to_str(30),
        help="Set gain [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=4,
        help="Set OverSampling Ratio [default=%default]")
    parser.add_option(
        "-p", "--ppm", dest="ppm", type="eng_float", default=eng_notation.num_to_str(0),
        help="Set ppm [default=%default]")
    parser.add_option(
        "-s", "--samp-rate", dest="samp_rate", type="eng_float", default=eng_notation.num_to_str(2000000.052982),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--serverport", dest="serverport", type="string", default="4729",
        help="Set UDP server listening port [default=%default]")
    parser.add_option(
        "-o", "--shiftoff", dest="shiftoff", type="eng_float", default=eng_notation.num_to_str(400e3),
        help="Set Frequency Shiftoff [default=%default]")
    return parser


def main(top_block_cls=grgsm_livemon, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(args=options.args, collector=options.collector, collectorport=options.collectorport, fc=options.fc, gain=options.gain, osr=options.osr, ppm=options.ppm, samp_rate=options.samp_rate, serverport=options.serverport, shiftoff=options.shiftoff)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
