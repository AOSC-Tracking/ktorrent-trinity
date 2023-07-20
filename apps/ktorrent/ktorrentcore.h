/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson                                   *
 *   joris.guisson@gmail.com                                               *
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
#ifndef KTORRENTCORE_H
#define KTORRENTCORE_H

#include <tqmap.h>
#include <tqtimer.h>
#include <tqcstring.h>
#include <util/constants.h>
#include <interfaces/coreinterface.h>

typedef TQValueList<TQCString> QCStringList;

namespace bt
{
	class Server;
	class QueueManager;
	class TorrentControl;
}

namespace TDEIO
{
	class Job;
}

namespace kt
{
	class Plugin;
	class PluginManager;
	class GUIInterface;
	class TorrentInterface;
	class GroupManager;
}




class KProgress;

/**
 * @author Joris Guisson
 * @brief Keeps track of all TorrentInterface objects
 *
 * This class keeps track of all TorrentInterface objects.
 */
class KTorrentCore : public kt::CoreInterface
{
	TQ_OBJECT
  
public:
	KTorrentCore(kt::GUIInterface* gui);
	virtual ~KTorrentCore();
	

	kt::PluginManager & getPluginManager() {return *pman;}
	const kt::PluginManager & getPluginManager() const {return *pman;}
	
	/**
	 * Load all torrents from the data dir.
	 */
	void loadTorrents();
	
	/**
	 * Load an existing torrent, which has already a properly set up torX dir.
	 * @param tor_dir The torX dir
	 */
	void loadExistingTorrent(const TQString & tor_dir);
	
	/**
	 * Set the maximum number of simultanious downloads.
	 * @param max The max num (0 == no limit)
	 */
	void setMaxDownloads(int max);
	
	/**
	 * Set the maximum number of simultaneous seeds.
	 * @param max The max num (0 == no limit)
	 */
	void setMaxSeeds(int max);
	
	/**
	 * Set wether or not we should keep seeding after
	 * a download has finished.
	 * @param ks Keep seeding yes or no
	 */
	void setKeepSeeding(bool ks);
	
	/**
	 * Change the data dir. This involves copying
	 * all data from the old dir to the new.
	 * This can offcourse go horribly wrong, therefore
	 * if it doesn't succeed it returns false
	 * and leaves everything where it supposed to be.
	 * @param new_dir The new directory
	 */
	bool changeDataDir(const TQString & new_dir);
	
	/**
	 * Save active torrents on exit.
	 */
	void onExit();

	/**
	 * Start all, takes into account the maximum number of downloads.
	 * @param type Wether to start all downloads or seeds. 1=Downloads, 2=Seeds, 3=All
	 */
	void startAll(int type);

	/**
	 * Stop all torrents.
	 * @param type Wether to start all downloads or seeds. 1=Downloads, 2=Seeds, 3=All
	 */
	void stopAll(int type);

	/**
	 * Make a torrent file
	 * @param file The file or dir to make a torrent of
	 * @param trackers A list of trackers
	 * @param chunk_size The size of each chunk (in KB)
	 * @param name The torrents name (usually filename)
	 * @param comments The comments
	 * @param seed Wether or not to start seeding or not
	 * @param output_file File to store the torrent file
	 * @param priv_tor Is this a private torrent
	 * @param prog Progress bar to update
	 */
	void makeTorrent(const TQString & file,const TQStringList & trackers,
					 int chunk_size,const TQString & name,const TQString & comments,
					 bool seed,const TQString & output_file,bool priv_tor,KProgress* prog, bool decentralized);

	CurrentStats getStats();

	/**
	 * Switch the port when no torrents are running.
	 * @param port The new port
	 * @return true if we can, false if there are torrents running
	 */
	bool changePort(bt::Uint16 port);

	///  Get the number of torrents running (including seeding torrents).
	bt::Uint32 getNumTorrentsRunning() const;

	///  Get the number of torrents not running.
	bt::Uint32 getNumTorrentsNotRunning() const;
	
	///Inserts blocked IP range into IPBlocklist
	void addBlockedIP(TQString& ip);
	
	///Removes blocked IP range from IPBlocklist
	void removeBlockedIP(TQString& ip);
	
	/**
	 * Find the next free torX dir.
	 * @return Path to the dir (including the torX part)
	 */
	TQString findNewTorrentDir() const;
	
	/**
	 * Load plugins. 
	 */
	void loadPlugins();
	
	virtual void load(const KURL& url);
	virtual void loadSilently(const KURL& url);
	virtual void loadSilentlyDir(const KURL& url, const KURL& savedir);
	virtual float getGlobalMaxShareRatio() const;
	
	
	bt::QueueManager* getQueueManager();
	
	kt::GroupManager* getGroupManager() const { return gman; }
	void setGroupManager(kt::GroupManager* g) { gman = g; }
	
	///Gets the number of torrents running
	int getNumRunning(bool onlyDownloads = true, bool onlySeeds = false) const;
	///Gets the number of torrents that are in state 'download' - total
	int countDownloads() const;
	///Gets the number of torrents that are in state 'seed' - total
	int countSeeds() const;
	
	int getMaxDownloadSpeed();
	int getMaxUploadSpeed();
	void setMaxDownloadSpeed(int v);
	void setMaxUploadSpeed(int v);

	void setPausedState(bool pause);
	
	kt::TorrentInterface* getTorFromNumber(int tornumber);
	TQValueList<int> getTorrentNumbers(int type);
	unsigned int getFileCount(int tornumber);
	QCStringList getFileNames(int tornumber);
	TQValueList<int> getFilePriorities(int tornumber);
	void setFilePriority(kt::TorrentInterface* tc, bt::Uint32 index, int priority);
	void announceByTorNum(int tornumber);


public slots:
	/**
	 * Load a torrent file. Pops up an error dialog
	 * if something goes wrong.
	 * @param file The torrent file (always a local file)
	 * @param dir Directory to save the data
	 * @param silently Wether or not to do this silently
	 */
	bool load(const TQString & file,const TQString & dir,bool silently);
	
	/**
	 * Load a torrent file. Pops up an error dialog
	 * if something goes wrong.
	 * @param data Byte array of the torrent file
	 * @param dir Directory to save the data
	 * @param silently Wether or not to do this silently
	 */
	bool load(const TQByteArray & data,const TQString & dir,bool silently, const KURL& url);
	
	/**
	 * Remove a download.This will delete all temp
	 * data from this TorrentInterface And delete the
	 * TorrentInterface itself. It can also potentially
	 * start a new download (when one is waiting to be downloaded).
	 * @param tc
	 * @param data_to 
	 */
	void remove(kt::TorrentInterface* tc,bool data_to);

	/**
	 * Update all torrents.
	 */
	void update();
	
		/**
	 * Start a torrent, takes into account the maximum number of downloads.
	 * @param tc The TorrentInterface
		 */
	void start(kt::TorrentInterface* tc);

	/**
	 * Stop a torrent, may start another download if it hasn't been started.
	 * @param tc The TorrentInterface
	 */
	void stop(kt::TorrentInterface* tc, bool user = false);
	
	/**
	 * Enqueue/Dequeue function. Places a torrent in queue. 
	 * If the torrent is already in queue this will remove it from queue.
	 * @param tc TorrentControl pointer.
	 */
	void queue(kt::TorrentInterface* tc);
	
	/**
	 * A torrent is about to be started. We will do some file checks upon this signal.
	 * @param tc The TorrentInterface
	*/
	void aboutToBeStarted(kt::TorrentInterface* tc,bool & ret);
	
	/**
	 * User tried to enqueue a torrent that has reached max share ratio.
	 * Emits appropriate signal.
	 */
	void enqueueTorrentOverMaxRatio(kt::TorrentInterface* tc);
	
	/**
	 * Do a data check on a torrent
	 * @param tc 
	 */
	void doDataCheck(kt::TorrentInterface* tc);
	
	///Fires when disk space is running low
	void onLowDiskSpace(kt::TorrentInterface* tc, bool stopped);
	
signals:
	/**
	* TorrentCore torrents have beed updated. Stats are changed.
	**/
	void statsUpdated();
	
	/**
	 * Emitted when a torrent has reached it's max share ratio.
	 * @param tc The torrent
	 */
	void maxShareRatioReached(kt::TorrentInterface* tc);
	
	/**
	 * Emitted when a torrent has reached it's max seed time
	 * @param tc The torrent
	 */
	void maxSeedTimeReached(kt::TorrentInterface* tc);
	
	/**
	 * Corrupted data has been detected.
	 * @param tc The torrent with the corrupted data
	 */
	void corruptedData(kt::TorrentInterface* tc);
	
	/**
	 * User tried to enqueue a torrent that has reached max share ratio. It's not possible.
	 * Signal should be connected to SysTray slot which shows appropriate KPassivePopup info.
	 * @param tc The torrent in question.
	 */
	void queuingNotPossible(kt::TorrentInterface* tc);
	
	/**
	 * Emitted when a torrent cannot be started
	 * @param tc The torrent
	 * @param reason The reason
	 */
	void canNotStart(kt::TorrentInterface* tc,kt::TorrentStartResponse reason);
	
	/**
	 * Diskspace is running low.
	 * Signal should be connected to SysTray slot which shows appropriate KPassivePopup info. 
	 * @param tc The torrent in question.
	 */
	void lowDiskSpace(kt::TorrentInterface* tc, bool stopped);

private:
	void rollback(const TQPtrList<kt::TorrentInterface> & success);
	void connectSignals(kt::TorrentInterface* tc);
	bool init(bt::TorrentControl* tc,bool silently);
	
private slots:
	void torrentFinished(kt::TorrentInterface* tc);
	void slotStoppedByError(kt::TorrentInterface* tc, TQString msg);
	void torrentSeedAutoStopped(kt::TorrentInterface* tc,kt::AutoStopReason reason);
	void downloadFinished(TDEIO::Job *job);
	void downloadFinishedSilently(TDEIO::Job *job);
	void emitCorruptedData(kt::TorrentInterface* tc);
	
private:
	TQString data_dir;
	int max_downloads;
	bool keep_seeding;
	TQTimer update_timer;
	bt::Uint64 removed_bytes_up,removed_bytes_down;
	kt::PluginManager* pman;
	bt::QueueManager* qman;
	kt::GroupManager* gman;
	TQMap<TDEIO::Job*,KURL> custom_save_locations; // map to store save locations
};

#endif
