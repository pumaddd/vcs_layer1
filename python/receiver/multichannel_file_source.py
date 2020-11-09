from gnuradio import gr
from gnuradio import blocks
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from grgsm import grd_config
from grgsm import pfb_channelizer_scanner
from grgsm import arfcn
from math import pi
from gnuradio.filter import optfir
import math
import grgsm
import numpy
import os
import logging

logger = logging.getLogger(__name__)
logger.setLevel(logging.WARNING)

class multi_channelize_layer(gr.hier_block2):
    __info__ = {
    'name': 'Multiple channelize layer',
    'device': 'MIDDLEWARE',
    'description': 'source run with received offline data read from database in /etc/data and output multiple 200k channel'
    }

    def __init__(self, input_fc_info=[], input_bandwidth_info=[], arfcns=[], bandwidth=0.2e6):
        _ninput = len(input_fc_info) if len(input_fc_info) < len(input_bandwidth_info) else len(input_bandwidth_info)
        _noutput = len(arfcns)

        gr.hier_block2.__init__(
            self, "multi channelize layer",
            gr.io_signature(_ninput, _ninput, gr.sizeof_gr_complex * 1),
            gr.io_signature(_noutput, _noutput, gr.sizeof_gr_complex * 1),
        )

        ##################################################
        # Parameters
        ##################################################
        logger.error("[CHANNELIZE LAYER]Initualize channelizer with module parameter {}".format(input_fc_info))
        self.arfcns = arfcns
        self.ninput = _ninput
        self.noutput = _noutput
        self.bandwidth_gsm = bandwidth
        self.OSR_PFB = 1
        self.OSR = 4

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_out = samp_rate_out = 1625000.0 / 6.0 * self.OSR
        self.inputs = [(input_fc_info[index], input_bandwidth_info[index]) for index in range(_ninput)]

        ##################################################
        # Blocks
        ##################################################
        self.channelizers = self.create_channelizers_map(inputs=self.inputs, arfcns=self.arfcns)

        ##################################################
        # Connections
        ##################################################
        port_offset = 0
        for index in range(len(self.channelizers)):
            channelizer = self.channelizers[index]
            self.connect((self, channelizer["portin"]), channelizer["channelizer"])    
            noutport = len(channelizer["arfcns"])
            for port in range(noutport):
                logger.debug("[CHANNELIZER]connect out put channel {} index {} to port out {}"
                        .format(channelizer, port, port_offset + port))
                self.connect((channelizer["channelizer"], port), (self, port_offset + port))
            port_offset += noutport

        ##################################################
        # Initualize Block
        ##################################################
    
    @staticmethod
    def create_channelizers_map(inputs=[(),], arfcns=[], band_size=0.2e6):
        channels_info = []
        map = []
        ninput = len(inputs)

        for index in range(ninput):
            input = inputs[index]
            channel_info = {
                    "fc":   input[0],
                    "bandwidth":    input[1],
                    "chan_num":     int(input[1]/band_size),
                    "arfcns":   [],
                    "outport":  [],
                    "portin": index,
                    }
            _start_fc = input[0] - input[1] / 2
            _stop_fc = input[0] + input[1] / 2
            num_channel = int(input[1]/0.2e6)
            channel_info["arfcns"] = [channel for channel in arfcns if \
                    (arfcn.arfcn2downlink(channel) > _start_fc and arfcn.arfcn2downlink(channel) < _stop_fc)]

            logger.debug("[CHANNELIZER]create channelize at input fc {} samplerate {} channel number {}"
                    .format(channel_info["fc"], channel_info["bandwidth"], channel_info["chan_num"]))
            _start_arfcn = arfcn.downlink2arfcn(input[0]) - num_channel/2
            _start_arfcn2 = arfcn.downlink2arfcn(_start_fc)
            channel_map = [arfcn_channel - _start_arfcn for arfcn_channel in channel_info["arfcns"]]
            # channel_map = [arfcn_channel - arfcn.downlink2arfcn(_start_fc) for arfcn_channel in channel_info["arfcns"]]
            logger.error("[CHANNELIZER]channel map {} start arfcn {} frequency".format(channel_info["arfcns"], _start_arfcn, _start_fc))
            channelizer = channelizer_ccf(
            chans_devide=channel_info["chan_num"],
            taps=(),
            oversample_rate=1,
            atten=100,
            scan_channels=channel_map)
            channelizer.set_channel_map(([]))
            channel_info.update({"channelizer": channelizer})
            channels_info.append(channel_info)
        
        # sort channels info accort fc center
        def fc_sort(e):
            print e["fc"]
            return e["fc"]
        channels_info.sort(key=fc_sort)
        logger.debug(channels_info)
        return channels_info

    def set_offset_byte(self, offset_byte):
        self.offset_byte = offset_byte

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr
        self.set_samp_rate_out(self.osr*self.gsm_symb_rate)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc-self.fc0)/self.samp_rate)
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)

    def get_list_fc0(self):
        fc0_list = []
        for channel_info in self.inputs:
            fc0_list.append(channel_info[0])
        
        return fc0_list

    def get_output_channel(self, fc):
        channel = {}
        arfcn = grgsm.arfcn.downlink2arfcn(fc)
        channel_info = self.get_fc_channel_info(fc)
        if channel_info == None:
            return None

        chan_num = int(channel_info[1] / self.bandwidth_gsm)
        arfcn_0 = grgsm.arfcn.downlink2arfcn(channel_info[0])
        arfcn_offsets = numpy.fft.ifftshift(numpy.array(range(int(-numpy.floor(chan_num / 2)), 
            int(numpy.floor((chan_num + 1) / 2)))))
        for index in xrange(0, chan_num):
            if arfcn_0 + arfcn_offsets[index] == arfcn:
                logger.debug("[CHANNELIZER]get fc {} with result {} from index {}"
                        .format(fc, channel_info[0], index))
                return (channel_info, index)
    
    def get_fc_channel_info(self, fc):
        channel = None
        for channel_info in self.inputs:
            _start_channel = channel_info[0] - channel_info[1]/2
            _stop_channel = channel_info[0] + channel_info[1]/2
            if fc > _start_channel and fc < _stop_channel:
                channel = channel_info
        return channel

    def center_frequency(self, fc):
        return arfcn.arfcn2downlink(arfcn.downlink2arfcn(fc))
    
    def set_ninput(self, ninput):
        ninput = self.ninput

    def get_noutput(self):
        noutput = self.noutput

    def lookup_port(self, port):
        for channelizer in self.channelizers: 
            for out_port in channelizer["outport"]:
                if port is out_port:
                    return channelizer["outport"].get(out_port)

    def destroy(self):
        self.disconnect_all() 
        logger.debug("[GSM CHANNELIZER] destroy object")
        for channelizer in self.channelizers:
            channelizer["channelizer"].destroy()
            del channelizer["channelizer"]
        del self.channelizers
        
class channelizer_ccf(gr.hier_block2):
    '''
    Make a Polyphase Filter channelizer (complex in, complex out, floating-point taps)

    This simplifies the interface by allowing a single input stream to connect to this block.
    It will then output a stream for each channel.
    '''
    def __init__(self, chans_devide, taps=None, oversample_rate=1, atten=100, scan_channels=[]):
        self.scan_chans = [channel for channel in scan_channels if (channel >= 0 and channel <= 63)]
        _nchansout = len(self.scan_chans)
  
        gr.hier_block2.__init__(self, "pfb_channelizer_ccf",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(_nchansout, _nchansout, gr.sizeof_gr_complex))
        self._oversample_rate = oversample_rate
        bitmask = 0x0
        fft_offsets = numpy.fft.fftshift(numpy.array(range(chans_devide)))
        for chan in self.scan_chans:
            mask = 0x01 << chan
            bitmask = bitmask | mask

        logger.error("[MULTIPLE CHANNELIZER]create channelizer with number channel {} for arfcn {} as mask {}"
                .format(chans_devide, scan_channels, bitmask))
        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(1, chans_devide, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, chans_devide)
        self.pfb = pfb_channelizer_scanner(chans_devide, self._taps,
                                              self._oversample_rate, bitmask)
        self.pfb.set_channel_map(fft_offsets)
        self.connect(self, self.s2ss)

        for i in xrange(chans_devide):
            self.connect((self.s2ss,i), (self.pfb,i))
        
        for i in xrange(_nchansout):
            self.connect((self.pfb,i), (self,i))

    def set_channel_map(self, newmap):
        self.pfb.set_channel_map(newmap)

    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def taps(self):
        return self.pfb.taps()

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)

    def destroy(self):
        logger.debug("[GSM CHANNELIZER CFF] destroy object")
        self.disconnect_all() 
        del self.s2ss
        del self.pfb
        del self._taps

