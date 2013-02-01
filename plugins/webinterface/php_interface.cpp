  /***************************************************************************
 *   Copyright (C) 2006 by Diego R. Brogna                                 *
 *   dierbro@gmail.com                                               	   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ***************************************************************************/

#include <tdeio/global.h>
#include <kglobal.h>
#include <klocale.h>

#include <net/socketmonitor.h>
#include <torrent/choker.h>
#include <torrent/udptrackersocket.h>
#include <kademlia/dhtbase.h>
#include <torrent/server.h>
#include <util/log.h>
#include <interfaces/functions.h>

#include "php_interface.h"
		
using namespace bt;

namespace kt
{
	extern TQString DataDir();
	
	using bt::FIRST_PRIORITY;
 	using bt::NORMAL_PRIORITY;
 	using bt::LAST_PRIORITY;
 	using bt::EXCLUDED;

	
	TQString BytesToString2(Uint64 bytes,int precision = 2)
	{
		TDELocale* loc = TDEGlobal::locale();
		if (bytes >= 1024 * 1024 * 1024)
			return TQString("%1 GB").arg(loc->formatNumber(bytes / TO_GIG,precision < 0 ? 2 : precision));
		else if (bytes >= 1024*1024)
			return TQString("%1 MB").arg(loc->formatNumber(bytes / TO_MEG,precision < 0 ? 1 : precision));
		else if (bytes >= 1024)
			return TQString("%1 KB").arg(loc->formatNumber(bytes / TO_KB,precision < 0 ? 1 : precision));
		else
			return TQString("%1 B").arg(bytes);
	}

	TQString KBytesPerSecToString2(double speed,int precision = 2)
	{
		TDELocale* loc = TDEGlobal::locale();
		return TQString("%1 KB/s").arg(loc->formatNumber(speed,precision));
	}

	/************************
	*PhpCodeGenerator	*
	************************/
	PhpCodeGenerator::PhpCodeGenerator(CoreInterface *c)
	{
		core=c;
	}
	
	/*Generate php code
	* function downloadStatus()
	* {
	*	return array( ... );
	* }
	*/
	void PhpCodeGenerator::downloadStatus(TQTextStream & out)
	{
		TorrentStats stats;
		//Priority file_priority;
		TQString status;
		out << "function downloadStatus()\n{\nreturn array(";

		TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
		for(int k=0; i != core->getQueueManager()->end(); i++, k++)
		{
			if (k > 0)
				out << ",\n";
			
			stats=(*i)->getStats();
			out << TQString("\n%1 => array(").arg(k);
			
			out << TQString("\"imported_bytes\" => %1,\n").arg(stats.imported_bytes);
			out << TQString("\"bytes_downloaded\" => \"%1\",\n").arg(BytesToString2(stats.bytes_downloaded));
			out << TQString("\"bytes_uploaded\" => \"%1\",\n").arg(BytesToString2(stats.bytes_uploaded));
			out << TQString("\"bytes_left\" => %1,\n").arg(stats.bytes_left);
			out << TQString("\"bytes_left_to_download\" => %1,\n").arg(stats.bytes_left_to_download);
			out << TQString("\"total_bytes\" => \"%1\",\n").arg(BytesToString2(stats.total_bytes));
			out << TQString("\"total_bytes_to_download\" => %1,\n").arg(stats.total_bytes_to_download);
			out << TQString("\"download_rate\" => \"%1\",\n").arg(KBytesPerSecToString2(stats.download_rate / 1024.0));
			out << TQString("\"upload_rate\" => \"%1\",\n").arg(KBytesPerSecToString2(stats.upload_rate / 1024.0));
			out << TQString("\"num_peers\" => %1,\n").arg(stats.num_peers);
			out << TQString("\"num_chunks_downloading\" => %1,\n").arg(stats.num_chunks_downloading);
			out << TQString("\"total_chunks\" => %1,\n").arg(stats.total_chunks);
			out << TQString("\"num_chunks_downloaded\" => %1,\n").arg(stats.num_chunks_downloaded);
			out << TQString("\"num_chunks_excluded\" => %1,\n").arg(stats.num_chunks_excluded);
			out << TQString("\"chunk_size\" => %1,\n").arg(stats.chunk_size);
			out << TQString("\"seeders_total\" => %1,\n").arg(stats.seeders_total);
			out << TQString("\"seeders_connected_to\" => %1,\n").arg(stats.seeders_connected_to);
			out << TQString("\"leechers_total\" => %1,\n").arg(stats.leechers_total);
			out << TQString("\"leechers_connected_to\" => %1,\n").arg(stats.leechers_connected_to);
			out << TQString("\"status\" => %1,\n").arg(stats.status);
			out << TQString("\"running\" => %1,\n").arg(stats.running);
			out << TQString("\"trackerstatus\" => \"%1\",\n").arg(stats.trackerstatus.replace("\\", "\\\\").replace("\"", "\\\"").replace("$", "\\$"));
			out << TQString("\"session_bytes_downloaded\" => %1,\n").arg(stats.session_bytes_downloaded);
			out << TQString("\"session_bytes_uploaded\" => %1,\n").arg(stats.session_bytes_uploaded);
			out << TQString("\"trk_bytes_downloaded\" => %1,\n").arg(stats.trk_bytes_downloaded);
			out << TQString("\"trk_bytes_uploaded\" => %1,\n").arg(stats.trk_bytes_uploaded);
			out << TQString("\"torrent_name\" => \"%1\",\n").arg(stats.torrent_name.replace("\\", "\\\\").replace("\"", "\\\"").replace("$", "\\$"));
			out << TQString("\"output_path\" => \"%1\",\n").arg(stats.output_path.replace("\\", "\\\\").replace("\"", "\\\"").replace("$", "\\$"));
			out << TQString("\"stopped_by_error\" => \"%1\",\n").arg(stats.stopped_by_error);
			out << TQString("\"completed\" => \"%1\",\n").arg(stats.completed);
			out << TQString("\"user_controlled\" => \"%1\",\n").arg(stats.user_controlled);
			out << TQString("\"max_share_ratio\" => %1,\n").arg(stats.max_share_ratio);
			out << TQString("\"priv_torrent\" => \"%1\",\n").arg(stats.priv_torrent);
			out << TQString("\"num_files\" => \"%1\",\n").arg((*i)->getNumFiles());			
			out << TQString("\"files\" => array(");
			out << flush;
			if (stats.multi_file_torrent)
			{
				//for loop to add each file+status to "files" array			
				for (Uint32 j = 0;j < (*i)->getNumFiles();j++)
				{
					if (j > 0)
						out << ",\n";
					
					TorrentFileInterface & file = (*i)->getTorrentFile(j);
					out << TQString("\"%1\" => array(\n").arg(j);
					out << TQString("\"name\" => \"%1\",\n").arg(file.getPath());
					out << TQString("\"size\" => \"%1\",\n").arg(TDEIO::convertSize(file.getSize()));
					out << TQString("\"perc_done\" => \"%1\",\n").arg(file.getDownloadPercentage());
					out << TQString("\"status\" => \"%1\"\n").arg(file.getPriority());
					out << TQString(")\n");
					out << flush;
				}
			}

			out << ")\n";
			out << ")\n";
		}
		
		out << ");\n}\n";
	}
	
	/*Generate php code
	* function globalStatus()
	* {
	*	return array( ... );
	* }
	*/
	void PhpCodeGenerator::globalInfo(TQTextStream & out)
	{
		out << "function globalInfo()\n{\nreturn array(";
		CurrentStats stats=core->getStats();
	
		out << TQString("\"download_speed\" => \"%1\",").arg(KBytesPerSecToString2(stats.download_speed / 1024.0));
		out << TQString("\"upload_speed\" => \"%1\",").arg(KBytesPerSecToString2(stats.upload_speed / 1024.0));
		out << TQString("\"bytes_downloaded\" => \"%1\",").arg(stats.bytes_downloaded);
		out << TQString("\"bytes_uploaded\" => \"%1\",").arg(stats.bytes_uploaded);
		out << TQString("\"max_download_speed\" => \"%1\",").arg(core->getMaxDownloadSpeed());
		out << TQString("\"max_upload_speed\" => \"%1\",").arg(core->getMaxUploadSpeed());
		out << TQString("\"max_downloads\" => \"%1\",").arg(Settings::maxDownloads());
		out << TQString("\"max_seeds\"=> \"%1\",").arg(Settings::maxSeeds());
		out << TQString("\"dht_support\" => \"%1\",").arg(Settings::dhtSupport());
		out << TQString("\"use_encryption\" => \"%1\"").arg(Settings::useEncryption());
		out << ");\n}\n";
	}
	
	
	/************************
	*PhpActionExec		*
	************************/
	PhpActionExec::PhpActionExec(CoreInterface *c)
	{
		core=c;
	}
	
	bool PhpActionExec::exec(KURL & url,bool & shutdown)
	{
		bool ret = false;
		shutdown = false;
		int separator_loc;
		TQString parse;
		TQString torrent_num;
		TQString file_num;
		KURL redirected_url;
		redirected_url.setPath(url.path());
		
		const TQMap<TQString, TQString> & params = url.queryItems();
		TQMap<TQString, TQString>::ConstIterator it;
	
		for ( it = params.begin(); it != params.end(); ++it ) 
		{
		//	Out(SYS_WEB| LOG_DEBUG) << "exec " << it.key().latin1() << endl;
			switch(it.key()[0])
			{
				case 'd':
					if(it.key()=="dht")
					{
						if(it.data()=="start")
						{
							Settings::setDhtSupport(true);
						}
						else
						{
							Settings::setDhtSupport(false);
						}
					
						dht::DHTBase & ht = Globals::instance().getDHT();
						if (Settings::dhtSupport() && !ht.isRunning())
						{
							ht.start(kt::DataDir() + "dht_table",kt::DataDir() + "dht_key",Settings::dhtPort());
							ret = true;
						}
						else if (!Settings::dhtSupport() && ht.isRunning())
						{
							ht.stop();
							ret = true;
						}
						else if (Settings::dhtSupport() && ht.getPort() != Settings::dhtPort())
						{
							ht.stop();
							ht.start(kt::DataDir() + "dht_table",kt::DataDir() + "dht_key",Settings::dhtPort());
							ret = true;
						}
					}	
					break;
				case 'e':
					if(it.key()=="encription")
					{
						if(it.data()=="start")
						{
							Settings::setUseEncryption(true);
						}
						else
						{
							Settings::setUseEncryption(false);
						}

						if (Settings::useEncryption())
						{
							Globals::instance().getServer().enableEncryption(Settings::allowUnencryptedConnections());
						}
						else
						{
							Globals::instance().getServer().disableEncryption();
						}
						ret = true;
					}
					break;
				case 'f':
					//parse argument into torrent number and file number
					separator_loc=it.data().find('-');
					parse=it.data();
					torrent_num.append(parse.left(separator_loc));
					file_num.append(parse.right(parse.length()-(separator_loc+1)));

					if(it.key()=="file_lp")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(torrent_num.toInt()==k)
							{
								TorrentFileInterface & file = (*i)->getTorrentFile(file_num.toInt());
								file.setPriority(LAST_PRIORITY);
								ret = true;
								break;
							}
						}
					}
					else if(it.key()=="file_np")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(torrent_num.toInt()==k)
							{
								TorrentFileInterface & file = (*i)->getTorrentFile(file_num.toInt());
								file.setPriority(NORMAL_PRIORITY);
								ret = true;
								break;
							}
						}
					}
					else if(it.key()=="file_hp")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(torrent_num.toInt()==k)
							{
								TorrentFileInterface & file = (*i)->getTorrentFile(file_num.toInt());
								file.setPriority(FIRST_PRIORITY);
								ret = true;
								break;
							}
						}
					}
					else if(it.key()=="file_stop")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(torrent_num.toInt()==k)
							{
								TorrentFileInterface & file = (*i)->getTorrentFile(file_num.toInt());
								file.setPriority(ONLY_SEED_PRIORITY);
								ret = true;
								break;
							}
						}
					}
					break;
				case 'g':
					if(it.key()=="global_connection")
					{
						Settings::setMaxTotalConnections(it.data().toInt());
						PeerManager::setMaxTotalConnections(Settings::maxTotalConnections());
						ret = true;
					}
					break;
				case 'l':
					if(it.key()=="load_torrent" && it.data().length() > 0)
					{
						core->loadSilently(KURL::decode_string(it.data()));
						ret = true;
					}
					break;
				case 'm':
					if(it.key()=="maximum_downloads")
					{
						core->setMaxDownloads(it.data().toInt());
						Settings::setMaxDownloads(it.data().toInt());
						ret = true;
					}
					else if(it.key()=="maximum_seeds")
					{
						core->setMaxSeeds(it.data().toInt());
						Settings::setMaxSeeds(it.data().toInt());	
						ret = true;
					}
					else if(it.key()=="maximum_connection_per_torrent")
					{
						PeerManager::setMaxConnections(it.data().toInt());
						Settings::setMaxConnections(it.data().toInt());
						ret = true;
					}
					else if(it.key()=="maximum_upload_rate")
					{
						Settings::setMaxUploadRate(it.data().toInt());
						core->setMaxUploadSpeed(Settings::maxUploadRate());
						net::SocketMonitor::setUploadCap( Settings::maxUploadRate() * 1024);
						ret = true;
					}
					else if(it.key()=="maximum_download_rate")
					{
						Settings::setMaxDownloadRate(it.data().toInt());
						core->setMaxDownloadSpeed(Settings::maxDownloadRate());
						net::SocketMonitor::setDownloadCap(Settings::maxDownloadRate()*1024);
						ret = true;
					}
					else if(it.key()=="maximum_share_ratio")
					{
						Settings::setMaxRatio(it.data().toInt());
						ret = true;
					}
					break;
				case 'n':
					if(it.key()=="number_of_upload_slots")
					{
						Settings::setNumUploadSlots(it.data().toInt());
						Choker::setNumUploadSlots(Settings::numUploadSlots());
						ret = true;
					}
					break;
				case 'p':
					if(it.key()=="port")
					{
						Settings::setPort(it.data().toInt());
						core->changePort(Settings::port());
					}
					else if(it.key()=="port_udp_tracker")
					{
						Settings::setUdpTrackerPort(it.data().toInt());
						UDPTrackerSocket::setPort(Settings::udpTrackerPort());
						ret = true;
					}
					break;
				case 'q':
					if(it.key()=="quit" && !it.data().isEmpty())
					{
						shutdown = true;
						ret = true;
					}
					break;
				case 'r':
					if(it.key()=="remove")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(it.data().toInt()==k)
							{
								core->remove((*i), false);
								ret = true;
								break;
							}
						}
					}
					break;
				case 's':
					if(it.key()=="stopall" && !it.data().isEmpty())
					{
						core->stopAll(3);
					}
					else if(it.key()=="startall" && !it.data().isEmpty())
					{
						core->startAll(3);
					}
					else if(it.key()=="stop")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(it.data().toInt()==k)
							{
								(*i)->stop(true);
								ret = true;
								break;
							}
						}
					}
					else if(it.key()=="start")
					{
						TQPtrList<TorrentInterface>::iterator i= core->getQueueManager()->begin();
						for(int k=0; i != core->getQueueManager()->end(); i++, k++)
						{
							if(it.data().toInt()==k)
							{
								(*i)->start();
								ret = true;
								break;
							}
						}
					}
					break;

				default:
					// add unknown query items to the redirected url
					// we don't add the keys above, because if the user presses refresh 
					// the same action will be taken again
					redirected_url.addQueryItem(it.key(),it.data());
					break;
			}
			Settings::writeConfig();	
		}
		
		if (ret)
			url = redirected_url; 
		
		return ret;
	}
	
	/************************
	*PhpInterface		*
	************************/
	PhpInterface::PhpInterface(CoreInterface *c):PhpCodeGenerator(c), PhpActionExec(c)
	{
	
	}

}
