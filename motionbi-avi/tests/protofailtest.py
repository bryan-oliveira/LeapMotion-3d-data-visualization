#!/bin/python

from subprocess import Popen, call
from time import sleep
import socket
from struct import pack

settings = {
    'exe_path' : 'bin/gprs',
    'port' : 12345,
    'host'   : '127.0.0.1'
}

state = {
    'proc' : None,
    'socket' : socket.socket(socket.AF_INET, socket.SOCK_DGRAM),
}

scores = ['correto', 'impreciso', 'incorreto']

def init():
    global tests
    tests = [
    {
        'id' : 'MBI-ST30FEAS',
        'func' : ST30FEAS,
        'description' : 'Pacotes com tipo não contemplado pelo protocolo especificado.',
        'expected_result' : 'Não deve existir alteração no estado do módulo AVI. (não “crasha” nem altera a visualização)'
    },
    {
        'id' : 'MBI-ST31FEAS',
        'func' : ST31FEAS,
        'description' : 'Envio de um pacote UDP sem dados.',
        'expected_result' : 'Não deve existir alteração no estado do módulo AVI. (não “crasha” nem altera a visualização)',
    }]

def restart():
    terminate()
    with open('/dev/null', 'w') as devnull:
        state['proc'] = Popen(settings['exe_path'], stdout=devnull, stderr=devnull)

def terminate():
    if( state['proc'] != None ):
        call(['kill', str(state['proc'].pid)]);
        state['proc'] = None

def send(packet):
    state['socket'].sendto(packet, (settings['host'], settings['port']))

def await_input():
    print('[pressiona <enter> para avançar]')
    input()

def ST30FEAS():

    results = []

    restart()
    print('PASSO #1 => Vou enviar um pacote com tipo 457.');
    await_input()
    send( pack('i', 457) )
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    restart()
    print('PASSO #2 => Vou enviar um pacote com tipo -457.');
    await_input()
    send( pack('i', -457) )
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')))
    
    return results


def ST31FEAS():

    restart()
    print('PASSO #1 => Vou enviar um pacote UDP vazio.')
    await_input()
    send( pack('') )
    print('Qual foi o resultado?')
    result = int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto'))
    
    return [result]



if __name__ == '__main__':
    init()

    for test in tests:
        print('[ID] ' + test['id'])
        print('[Descrição]\n' + test['description'])
        print('[Resultado Esperado]\n' + test['expected_result'])
        test['result'] = test['func']()

        print('Teste terminado. pressiona <enter> para seguir ao próximo teste.')
        await_input()

    for test in tests:
        out = test['id'] + '\t' + scores[ max(test['result'])-1 ]
        if( max(test['result']) == 3 ):
            print(out + '\tPassos Incorretos: ' + ' '.join([ str(i+1) for i in range(len(test['result'])) if test['result'][i] == max(test['result']) ]))
        else:
            print(out)

