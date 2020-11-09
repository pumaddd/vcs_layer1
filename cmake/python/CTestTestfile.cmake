# CMake generated Testfile for 
# Source directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python
# Build directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(qa_arfcn "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_arfcn_test.sh")
add_test(qa_decryption "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_decryption_test.sh")
add_test(qa_burst_printer "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_printer_test.sh")
add_test(qa_message_printer "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_message_printer_test.sh")
add_test(qa_burst_file_source "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_file_source_test.sh")
add_test(qa_burst_timeslot_splitter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_timeslot_splitter_test.sh")
add_test(qa_burst_sdcch_subslot_splitter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_sdcch_subslot_splitter_test.sh")
add_test(qa_burst_timeslot_filter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_timeslot_filter_test.sh")
add_test(qa_burst_sdcch_subslot_filter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_sdcch_subslot_filter_test.sh")
add_test(qa_burst_fnr_filter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_burst_fnr_filter_test.sh")
add_test(qa_dummy_burst_filter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_dummy_burst_filter_test.sh")
add_test(qa_tch_f_decoder "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_tch_f_decoder_test.sh")
add_test(qa_tch_h_decoder "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_tch_h_decoder_test.sh")
add_test(qa_tch_f_chans_demapper "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_tch_f_chans_demapper_test.sh")
add_test(qa_tch_h_chans_demapper "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_tch_h_chans_demapper_test.sh")
add_test(qa_txtime_setter "/bin/sh" "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/qa_txtime_setter_test.sh")
subdirs("misc_utils")
subdirs("receiver")
subdirs("demapping")
subdirs("transmitter")
subdirs("trx")
