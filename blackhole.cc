#include "ns3/aodv-module.h"
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/mobility-module.h"
#include "myapp.h"

NS_LOG_COMPONENT_DEFINE ("Blackhole");

using namespace ns3;



void
ReceivePacket(Ptr<const Packet> p, const Address & addr)
{
	std::cout << Simulator::Now ().GetSeconds () << "\t" << p->GetSize() <<"\n";
}


int main (int argc, char *argv[])
{
  bool enableFlowMonitor = false;
  std::string phyMode ("DsssRate1Mbps");

  CommandLine cmd;
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.Parse (argc, argv);

//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c; // ALL Nodes
  NodeContainer not_malicious;
  NodeContainer malicious;
  c.Create(25);

  not_malicious.Add(c.Get(0));
  not_malicious.Add(c.Get(1));
  not_malicious.Add(c.Get(2));
  not_malicious.Add(c.Get(3));
  not_malicious.Add(c.Get(4));
  not_malicious.Add(c.Get(5));
  not_malicious.Add(c.Get(6));
  malicious.Add(c.Get(7));
  not_malicious.Add(c.Get(8));
  not_malicious.Add(c.Get(9));
  not_malicious.Add(c.Get(10));
  not_malicious.Add(c.Get(11));
  not_malicious.Add(c.Get(12));
  malicious.Add(c.Get(13));
  not_malicious.Add(c.Get(14));
  malicious.Add(c.Get(15));
  malicious.Add(c.Get(16));
  not_malicious.Add(c.Get(17));
  not_malicious.Add(c.Get(18));
  not_malicious.Add(c.Get(19));
  not_malicious.Add(c.Get(20));
  not_malicious.Add(c.Get(21));
  malicious.Add(c.Get(22));
  not_malicious.Add(c.Get(23));
  not_malicious.Add(c.Get(24));
  // Set up WiFi
  WifiHelper wifi;

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

  YansWifiChannelHelper wifiChannel ;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
	  	  	  	  	  	  	  	    "SystemLoss", DoubleValue(1),
		  	  	  	  	  	  	    "HeightAboveZ", DoubleValue(1.5));

  // For range near 250m
  wifiPhy.Set ("TxPowerStart", DoubleValue(33));
  wifiPhy.Set ("TxPowerEnd", DoubleValue(33));
  wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
  wifiPhy.Set ("TxGain", DoubleValue(0));
  wifiPhy.Set ("RxGain", DoubleValue(0));
  wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));
  wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));

  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add a non-QoS upper mac
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");

  // Set 802.11b standard
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue(phyMode),
                                "ControlMode",StringValue(phyMode));


  NetDeviceContainer devices;
  devices = wifi.Install (wifiPhy, wifiMac, c);


//  Enable AODV
  AodvHelper aodv;
  AodvHelper malicious_aodv; 
 

  // Set up internet stack
  InternetStackHelper internet;
  internet.SetRoutingHelper (aodv);
  internet.Install (not_malicious);
  
  malicious_aodv.Set("IsMalicious",BooleanValue(true)); // putting *false* instead of *true* would disable the malicious behavior of the node
  internet.SetRoutingHelper (malicious_aodv);
  internet.Install (malicious);

  // Set up Addresses
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);


  NS_LOG_INFO ("Create Applications.");

  // UDP connection from N1 to N3

  uint16_t sinkPort = 26;
  Address sinkAddress (InetSocketAddress (ifcont.GetAddress (24), sinkPort)); // interface of n3
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get (24)); //n3 as sink
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (c.Get (1), UdpSocketFactory::GetTypeId ()); //source at n1

  // Create UDP application at n1
  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3UdpSocket, sinkAddress, 1040, 5, DataRate ("250Kbps"));
  c.Get (1)->AddApplication (app);
  app->SetStartTime (Seconds (40.));
  app->SetStopTime (Seconds (100.));

// Set Mobility for all nodes

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
  positionAlloc ->Add(Vector(230, 230, 0)); // node0
  positionAlloc ->Add(Vector(160, 170, 0)); // node1 
  positionAlloc ->Add(Vector(300, 100, 0)); // node2
  positionAlloc ->Add(Vector(420, 50, 0)); // node3
  positionAlloc ->Add(Vector(620, 180, 0)); // node4
  positionAlloc ->Add(Vector(820, 120, 0)); // node5
  positionAlloc ->Add(Vector(740, 230, 0)); // node6 
  positionAlloc ->Add(Vector(220, 340, 0)); // node7
  positionAlloc ->Add(Vector(520, 240, 0)); // node8
  positionAlloc ->Add(Vector(430, 250, 0)); // node9
  positionAlloc ->Add(Vector(310, 280, 0)); // node10
  positionAlloc ->Add(Vector(450, 330, 0)); // node11 
  positionAlloc ->Add(Vector(260, 430, 0)); // node12
  positionAlloc ->Add(Vector(190, 300, 0)); // node13
  positionAlloc ->Add(Vector(310, 340, 0)); // node14
  positionAlloc ->Add(Vector(470, 470, 0)); // node15
  positionAlloc ->Add(Vector(390, 420, 0)); // node16 
  positionAlloc ->Add(Vector(450, 600, 0)); // node17
  positionAlloc ->Add(Vector(580, 520, 0)); // node18
  positionAlloc ->Add(Vector(550, 350, 0)); // node19
  positionAlloc ->Add(Vector(620, 550, 0)); // node20
  positionAlloc ->Add(Vector(520, 450, 0)); // node21 
  positionAlloc ->Add(Vector(640, 380, 0)); // node22
  positionAlloc ->Add(Vector(820, 390, 0)); // node23
  positionAlloc ->Add(Vector(840, 580, 0)); // node24

  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(c);

  AnimationInterface anim ("blackhole1.xml"); // Mandatory
  AnimationInterface::SetConstantPosition (c.Get (0), 230, 230);
  AnimationInterface::SetConstantPosition (c.Get (1), 160, 170);
  AnimationInterface::SetConstantPosition (c.Get (2), 300, 100);
  AnimationInterface::SetConstantPosition (c.Get (3), 420, 50); 
  AnimationInterface::SetConstantPosition (c.Get (4), 620, 180);
  AnimationInterface::SetConstantPosition (c.Get (5), 820, 120);
  AnimationInterface::SetConstantPosition (c.Get (6), 740, 230);
  AnimationInterface::SetConstantPosition (c.Get (7), 220, 340);
  AnimationInterface::SetConstantPosition (c.Get (8), 520, 240); 
  AnimationInterface::SetConstantPosition (c.Get (9), 430, 250);
  AnimationInterface::SetConstantPosition (c.Get (10), 310, 280);
  AnimationInterface::SetConstantPosition (c.Get (11), 450, 330);
  AnimationInterface::SetConstantPosition (c.Get (12), 260, 430);
  AnimationInterface::SetConstantPosition (c.Get (13), 190, 300); 
  AnimationInterface::SetConstantPosition (c.Get (14), 310, 340);
  AnimationInterface::SetConstantPosition (c.Get (15), 470, 470);
  AnimationInterface::SetConstantPosition (c.Get (16), 390, 420);
  AnimationInterface::SetConstantPosition (c.Get (17), 450, 600);
  AnimationInterface::SetConstantPosition (c.Get (18), 580, 520); 
  AnimationInterface::SetConstantPosition (c.Get (19), 550, 350);
  AnimationInterface::SetConstantPosition (c.Get (20), 620, 550);
  AnimationInterface::SetConstantPosition (c.Get (21), 520, 450);
  AnimationInterface::SetConstantPosition (c.Get (22), 640, 380);
  AnimationInterface::SetConstantPosition (c.Get (23), 820, 390); 
  AnimationInterface::SetConstantPosition (c.Get (24), 840, 580);
  anim.EnablePacketMetadata(true);

      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("blackhole.routes", std::ios::out);
      aodv.PrintRoutingTableAllAt (Seconds (45), routingStream);

  // Trace Received Packets
  Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback (&ReceivePacket));


//
// Calculate Throughput using Flowmonitor
//
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();


//
// Now, do the actual simulation.
//

  AsciiTraceHelper ascii;
  wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("blackhole3.tr"));

  wifiPhy.EnablePcapAll ("blackhole3");

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds(100.0));
  Simulator::Run ();

  monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress=="10.1.2.1" && t.destinationAddress == "10.1.2.25"))
      {
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      	  std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      }
     }

  monitor->SerializeToXmlFile("lab-4.flowmon", true, true);
  return 1;

}
