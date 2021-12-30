<?php
// Usage: tcpdump -qnnttl [options] | php iptraf.php

class Stats {
	public $bDown, $bUp;
	public $cDown, $cUp;

	function __construct($bDown = 0, $bUp = 0, $pDown = 0, $pUp = 0) {
		$this->bDown = $bDown;
		$this->bUp = $bUp;
		$this->pDown = $pDown;
		$this->pUp = $pUp;
	}

	function add($stats) {
		$this->bDown += $stats->bDown;
		$this->bUp += $stats->bUp;
		$this->pDown += $stats->pDown;
		$this->pUp += $stats->pUp;
	}

	function out($str, $factor) {
		printf("%16s %16.2f %16.2f %16.2f %16.2f\n",
				$str,
				$this->bDown / 1024 / $factor,
				$this->pDown / 1000 / $factor,
				$this->bUp / 1024 / $factor,
				$this->pUp / 1000 / $factor);
	}
}

class Parser {
	protected $lastTime;
	protected $stats = array();
	protected $delay = 2;

	function trafficCmp($a, $b) {
		return $b->bDown - $a->bDown;
	}

	function printStats() {
		if (empty($this->stats))
			return;
		uasort($this->stats["IP"], array($this, "trafficCmp"));
		echo "\033[2J\033[1;1H";
		echo "           Every " . $this->delay . " seconds. Current time: " .
			date("r") . "\n\n";
		printf("%16s %16s %16s %16s %16s\n\n",
				"IP",
				"RX kbytes/s",
				"RX kpackets/s",
				"TX kbytes/s",
				"TX kpackets/s");
		$total = new Stats();
		foreach ($this->stats["IP"] as $ip => $stats) {
			$stats->out($ip, $this->delay);
			$total->add($stats);
		}
		echo "\n";
		$total->out("TOTAL", $this->delay);
	}

	function match($ip) {
		return substr($ip, 0, 5) == "10.1.";
	}

	function parse($line) {
		list($time, $l3proto, $data) = explode(' ', $line, 3);
		$time = explode('.', $time);
		$curTime = (int)floor($time[0] / $this->delay);

		if ($curTime != $this->lastTime) {
			$this->printStats();
			$this->lastTime = $curTime;
			$this->stats = array();
		}
		if (!isset($this->stats[$l3proto]))
			$this->stats[$l3proto] = array();
		$stats =& $this->stats[$l3proto];

		switch ($l3proto) {
		case 'IP':
			list($ip, $data) = explode(': ', $data);
			$ip = explode(' > ', $ip);
			$ip[0] = implode('.', explode('.', $ip[0], -1));
			$ip[1] = implode('.', explode('.', $ip[1], -1));
			if ($this->match($ip[0])) {
				$ip = $ip[0];
				$dir = "Up";
			} elseif ($this->match($ip[1])) {
				$ip = $ip[1];
				$dir = "Down";
			} else
				break;

			if (substr($data, 0, 4) == "tcp ") {
				$data = explode(' ', $data);
				$len = $data[1];
			} elseif (substr($data, 0, 4) == "UDP,") {
				$data = explode(' ', $data);
				$len = $data[2];
			} else {
				//echo "Ignoring unknown IP proto: " . $data;
				break;
			}

			if (!isset($stats[$ip]))
				$stats[$ip] = new Stats();

			$bm = "b" . $dir;
			$pm = "p" . $dir;
			$stats[$ip]->$bm += $len;
			$stats[$ip]->$pm ++;
			break;
		default:
			//echo "Ignoring unknown proto '" . $l3proto . "'\n";
		}
	}
}

$fp = fopen("php://stdin", "r");
$parser = new Parser();
while ($line = fgets($fp))
	$parser->parse($line);
fclose ($fp);

?>
