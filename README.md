## What’s this?
A proof-of-concept of a VPN implementation over the IRC protocol.

## Come again?
A computer running this daemon will create a tunnel network device and connect to the specified IRC server. Any packet transmitted to the tunnel device will be read by the daemon, encoded in base64 and sent to the specified user using an IRC private message. The other side running the daemon will read the encoded base64 packet, decode it to binary and write it to its tunnel interface. The two sides running the daemon can transfer any application level protocol without direct communication with each other.

## Can I do something productive with it?
Probably no. The IRC protocol doesn’t allow sending private messages longer than 512 characters. This means that the MTU of the tunnel interface should be around 333 bytes. Most IRC servers will kick you because of flooding if you try to transmit anything faster than ICMP pings. Even servers that don’t have flood protection have serious lags in transmitting messages, which make most protocols useless under this limitation. The only thing which I managed to transfer safely using this VPN is ICMP ping.

## Is it secure?
No. Packages are transmitted to the other side without any encryption. There’s also no method to authenticate the other side.

## What operating systems are supported?
Linux only. Other Unix variations are not supported at the moment.

## How can I test it?
The recommended way is to use Vagrant. Running `vagrant up` will create two virtual machines named Alice and Bob, both have access to the source directory. Alice has the tools needed to build the project and Bob can run the compiled binary. In Alice, you should run `cmake .` and then `make`. The compiled executable can be also run by Bob. When the daemon is running in both Alice and Bob, try sending a ping from one side to the other using the VPN’s IP address.

## Licence
Do whatever you want with it.
