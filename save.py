import sys
import pymongo
import os
import csv
import datetime

def read_files_into_dict(path):
    files = {}
    
    for node in os.listdir(path):
        if os.path.isfile(os.path.join(path, node)):
            with open(os.path.join(path, node), 'rb') as file:
                files[node] = file.read()
    
    return files

def generate_document_from_path(path):
    doc = {}
        
    doc['crashes'] = read_files_into_dict(os.path.join(path, 'crashes'))
    doc['crashes'].pop('README.txt')
    doc['hangs'] = read_files_into_dict(os.path.join(path, 'hangs'))
    doc['queue'] = read_files_into_dict(os.path.join(path, 'queue'))

    with open(os.path.join(path, 'cmdline')) as cmdline:
        doc['cmdline'] = cmdline.read().splitlines()
        
    with open(os.path.join(path, 'fuzz_bitmap'), 'rb') as fuzz_bitmap:
        doc['fuzz_bitmap'] = fuzz_bitmap.read()

    with open(os.path.join(path, 'fuzzer_stats')) as fuzzer_stats:
        doc['fuzzer_stats'] = {}
        lines = fuzzer_stats.read().splitlines()
        for line in lines:
            stat = line.split(':', 1)
            doc['fuzzer_stats'][stat[0].strip()] = stat[1].strip()
        
    with open(os.path.join(path, 'plot_data')) as plot_data:
        plot_data_csv = csv.reader(plot_data, skipinitialspace=True)
        doc['plot_data'] = list(plot_data_csv)
    
    return doc

def main():
    if len(sys.argv) == 1 or (sys.argv[1] != 'ld' and sys.argv[1] != 'gold' and sys.argv[1] != 'lld'):
        print('Usage: save.py <linker>, where linker is either "ld" or "gold".\n')
        return
        
    linker = sys.argv[1]

    client = pymongo.MongoClient()
    ftl_afl = client['ftl_afl']
    collection = ftl_afl[linker]
    
    findings_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'findings_' + linker)
    doc = generate_document_from_path(findings_path)
    doc['_id'] = datetime.datetime.utcnow()
    collection.insert(doc, check_keys=False)

if __name__ == '__main__':
    main()

