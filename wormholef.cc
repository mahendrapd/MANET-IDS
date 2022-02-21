/* Wormhole Attack Simulation with AODV Routing Protocol - Sample Program
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

NS_LOG_COMPONENT_DEFINE ("Wormhole");

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

   Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Mode", StringValue ("Time"));
   Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Time", StringValue ("2s"));
   Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
   Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Bounds", StringValue ("0|500|0|500"));

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
  not_malicious.Add(c.Get(6));
  not_malicious.Add(c.Get(7));
  not_malicious.Add(c.Get(8));
  not_malicious.Add(c.Get(9));

  not_malicious.Add(c.Get(10));
  not_malicious.Add(c.Get(11));
  not_malicious.Add(c.Get(12));
  not_malicious.Add(c.Get(13));
  not_malicious.Add(c.Get(14));

  not_malicious.Add(c.Get(15));
  not_malicious.Add(c.Get(16));
  not_malicious.Add(c.Get(17));
  not_malicious.Add(c.Get(18));
  not_malicious.Add(c.Get(19));

  not_malicious.Add(c.Get(20));
  not_malicious.Add(c.Get(21));
  not_malicious.Add(c.Get(22));
  not_malicious.Add(c.Get(23));
  not_malicious.Add(c.Get(24));

  malicious.Add(c.Get(1));
  malicious.Add(c.Get(2));
  malicious.Add(c.Get(3));
  malicious.Add(c.Get(4));
  malicious.Add(c.Get(5));

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


  NetDeviceContainer devices, mal_devices;
  devices = wifi.Install (wifiPhy, wifiMac, c);
  mal_devices = wifi.Install(wifiPhy, wifiMac, malicious);

//  Enable AODV
  AodvHelper aodv;
  AodvHelper malicious_aodv; 
 

  // Set up internet stack
  InternetStackHelper internet;
  internet.SetRoutingHelper (aodv);
  internet.Install (not_malicious);
  /*
  malicious_aodv.Set("EnableWrmAttack",BooleanValue(true)); // putting *false* instead of *true* would disable the malicious behavior of the node

  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.2"));//10.0.1.2 for node 1
  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.3"));//10.0.1.3 for node 2
  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.4"));//10.0.1.2 for node 3
  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.5"));//10.0.1.3 for node 4
  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.6"));//10.0.1.2 for node 5
*/
  internet.SetRoutingHelper (malicious_aodv);
  internet.Install (malicious);

  // Set up Addresses
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer mal_ifcont = ipv4.Assign (mal_devices);



  NS_LOG_INFO ("Create Applications.");

  // UDP connection from N0 to N3

  uint16_t sinkPort = 25;
  Address sinkAddress (InetSocketAddress (ifcont.GetAddress (24), sinkPort)); // interface of n3
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get (24)); //n3 as sink
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (c.Get (0), UdpSocketFactory::GetTypeId ()); //source at n0

  // Create UDP application at n0
  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3UdpSocket, sinkAddress, 1040, 25, DataRate ("250Kbps"));
  c.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (40.));
  app->SetStopTime (Seconds (100.));

// Set Mobility for all nodes

  MobilityHelper mobility;

  Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
  positionAlloc ->Add(Vector(200, 250, 0)); // node0
  positionAlloc ->Add(Vector(300, 300, 0)); // node1 
  positionAlloc ->Add(Vector(400, 500, 0)); // node2
  positionAlloc ->Add(Vector(500, 220, 0)); // node3
  positionAlloc ->Add(Vector(600, 400, 0)); // node4
  positionAlloc ->Add(Vector(800, 410, 0)); // node5

  positionAlloc ->Add(Vector(400, 410, 0)); // node6
  positionAlloc ->Add(Vector(280, 180, 0)); // node7 
  positionAlloc ->Add(Vector(220, 400, 0)); // node8
  positionAlloc ->Add(Vector(350, 420, 0)); // node9
  positionAlloc ->Add(Vector(580, 600, 0)); // node10
  positionAlloc ->Add(Vector(420, 420, 0)); // node11

  positionAlloc ->Add(Vector(320, 220, 0)); // node12
  positionAlloc ->Add(Vector(400, 100, 0)); // node13 
  positionAlloc ->Add(Vector(580, 240, 0)); // node14
  positionAlloc ->Add(Vector(650, 250, 0)); // node15
  positionAlloc ->Add(Vector(800, 200, 0)); // node16
  positionAlloc ->Add(Vector(700, 430, 0)); // node17

  positionAlloc ->Add(Vector(580, 500, 0)); // node18
  positionAlloc ->Add(Vector(700, 400, 0)); // node19 
  positionAlloc ->Add(Vector(800, 350, 0)); // node20
  positionAlloc ->Add(Vector(700, 550, 0)); // node21
  positionAlloc ->Add(Vector(870, 570, 0)); // node22
  positionAlloc ->Add(Vector(850, 400, 0)); // node23
  positionAlloc ->Add(Vector(900, 500, 0)); // node24

  mobility.SetPositionAllocator(positionAlloc);
  /*mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (0.0),
                                  "MinY", DoubleValue (0.0),
                                  "DeltaX", DoubleValue (100),
                                  "DeltaY", DoubleValue (50),
                                  "GridWidth", UintegerValue (6),
                                  "LayoutType", StringValue ("RowFirst")); */
  //mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  //mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel");
  /*mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                  "X", StringValue ("700.0"),
                                  "Y", StringValue ("300.0"),
                                  "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
   mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                              "Mode", StringValue ("Time"),
                              "Time", StringValue ("2s"),
                              "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                              "Bounds", StringValue ("0|1000|0|1000"));*/
  mobility.Install(c);
/*

  AnimationInterface anim ("wormhole.xml"); // Mandatory
  AnimationInterface::SetConstantPosition (c.Get (0), 0, 500);
  AnimationInterface::SetConstantPosition (c.Get (1), 200, 500);
  AnimationInterface::SetConstantPosition (c.Get (2), 400, 500);
  AnimationInterface::SetConstantPosition (c.Get (3), 600, 500); 
  AnimationInterface::SetConstantPosition (c.Get (4), 200, 600);
  AnimationInterface::SetConstantPosition (c.Get (5), 400, 600); 
  AnimationInterface::SetConstantPosition (c.Get (6), 350, 600);
  AnimationInterface::SetConstantPosition (c.Get (7), 300, 500);

  anim.EnablePacketMetadata(true);
*/
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("wormhole.routes", std::ios::out);
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
  wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("wormhole1.tr"));

  wifiPhy.EnablePcapAll ("wormhole1");

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds(100.0));
  Simulator::Run ();

  monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress=="10.0.1.1" && t.destinationAddress == "10.0.1.25"))
      {
          std::cout << "  Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      	  std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      }
     }

  monitor->SerializeToXmlFile("lab-4.flowmon", true, true);

  return 0;
}
